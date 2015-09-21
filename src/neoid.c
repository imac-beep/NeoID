#include "nid_core.h"
#include "neoid.h"
#include "nid_server.h"

static void usage(const char *file);
static int32_t nid_option_init(int argc, char *argv[], struct opt_data *optdata);
static int32_t nid_deamon_init(void);


int main(int argc, char *argv[]) {
    struct opt_data opt;
    if (NID_ERROR == nid_option_init(argc, argv, &opt)) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    printf("[Start with: machine_id = %d, port = %d, backlog = %d]\n", opt.machine_id, opt.port, opt.backlog);

    if (opt.deamonize && NID_ERROR == nid_deamon_init()) {
        return EXIT_FAILURE;
    }

    if (NID_ERROR == nid_server_start(opt.port, opt.backlog, opt.machine_id)) {
        puts("NeoID server start failed!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static void usage(const char *file) {
    printf("Usage: %s [-i machine_id] [-p port] [-b backlog] [-d]\n"
                   "  -i machine_id        : Set this server id(0 - 127). The default machine_id: %d\n"
                   "  -p port              : Set server port(1 - 65535). The default port: %d\n"
                   "  -b backlog           : Set server connection backlog. The default backlog: %d\n"
                   "  -d                   : Run server in background.\n"
                   ,file, NID_MACHINE_ID, NID_PORT, NID_BACKLOG);
}

static int32_t nid_option_init(int argc, char *argv[], struct opt_data *optdata) {
    int32_t opt;

    if (NULL == optdata) {
        return NID_ERROR;
    }

    optdata->machine_id = NID_MACHINE_ID;
    optdata->port = NID_PORT;
    optdata->backlog = NID_BACKLOG;
    optdata->deamonize = NID_FALSE;
    while (NID_ERROR != (opt = getopt(argc, argv, "i:p:b:d"))) {
        switch (opt) {
            case 'i':
                optdata->machine_id = (uint8_t) atoi(optarg);
                break;
            case 'p':
                optdata->port = (uint16_t) atoi(optarg);
                break;
            case 'b':
                optdata->backlog = (uint32_t) atoi(optarg);
                break;
            case 'd':
                optdata->deamonize = NID_TRUE;
                break;
            default:
                return NID_ERROR;
        }
    }

    if (optdata->machine_id < 0 || optdata->machine_id > 0x7F ||
        optdata->port < 1 || optdata->port > 0xFFFF ||
        optdata->backlog < 1) {
        return NID_ERROR;
    }

    return NID_OK;
}

static int32_t nid_deamon_init(void) {
    nid_bool_t forked = NID_FALSE;

    do {
        switch (fork()) {
            case NID_ERROR:
                perror("Deamon");
                return NID_ERROR;
            case 0:
                break;
            default:
                exit(EXIT_SUCCESS);
        }

        if (forked) {
            break;
        }

        if (NID_ERROR == setsid()) {
            perror("SetSid");
            return NID_ERROR;
        }

        forked = NID_TRUE;
    } while(NID_TRUE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    if (NID_ERROR == chdir("/")) {
        perror("Chdir");
        return NID_ERROR;
    }
    umask(0);

    return NID_OK;
}
