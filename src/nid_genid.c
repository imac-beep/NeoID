#include "nid_core.h"
#include "nid_genid.h"

static void nid_genid_timer(int signo);

static uint32_t subid = 0x00FFFFFF;
static nid_bool_t resettime = NID_FALSE;
static time_t timestamp;
static uint64_t tempid;
static uint64_t machine_id;

extern uint64_t nid_genid_new() {
    if (resettime || ++subid > 0x00FFFFFF) { //如果id尾部(24 bits)溢出，读秒并轮转
        /**
         * 如果跑的过快（一秒钟跑了超过一千六百万条），则等到下一秒
         * 该部位代码应该没什么可能被触发到。
         */
        while (!resettime) {
            usleep(1000); //sleep 1ms
        }

        subid = 0;
        resettime = NID_FALSE;

        if (timestamp > 0xFFFFFFFF) {
            /**
             * 超过32位的情况。该部位代码应该没什么可能被触发到。
             * 魔法数字由来：2015-07-07 00:00:00 的时间戳 加 0xFFFFFFFF，用来判断轮转溢出。
             * 如果超过32位时间，会重新轮转到1。
             * 但如果轮转的时间超过2151-08-13 06:28:15 则系统无法使用。
             * 但我想最起码一百多年过去了，更宽的位和更好的架构肯定出现了。
             */
            if (timestamp > 0x1559AA5FF) {
                return 0;
            }
            timestamp -= 0xFFFFFFFF;
            tempid = (uint64_t)timestamp << 31 | machine_id;
        }
    }

    return tempid | subid;
}

static void nid_genid_timer(int signo) {
    if (SIGALRM == signo) {
        timestamp = time(NULL);
        tempid = (uint64_t)timestamp << 31 | machine_id;
        resettime = NID_TRUE;
    }
}

extern int32_t nid_genid_init(uint8_t mid) {
    struct sigaction sa;
    struct itimerval itv;

    machine_id = (uint64_t)mid << 24;

    sa.sa_flags = 0;
    sa.sa_handler = nid_genid_timer;
    sigemptyset(&sa.sa_mask);
    if (NID_ERROR == sigaction(SIGALRM, &sa, NULL)) {
        return NID_ERROR;
    }

    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    if (NID_ERROR == setitimer(ITIMER_REAL, &itv, NULL)) {
        return NID_ERROR;
    }

    return NID_OK;
}
