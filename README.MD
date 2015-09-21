# NeoID #
### 一个支持分布式部署的ID生成服务程序。支持 linux 和 unix。###

**编译安装：**<br />
直接代码根目录下 make 即可（需要安装cmake）
make 完毕可以在 build/bin/neoid 找到可执行的服务程序。

**启动运行：**<br />
./neoid 直接启动
    
**Usage:** neoid [-i machine_id] [-p port] [-b backlog] [-d]<br />
 **-i machine_id :** 服务实例的ID号，可支持0 - 127 共128个实例。默认为：0，不要重复。<br />
 **-p port :** 服务所使用的端口，1024以下端口需要root权限。默认为：18989，注意防火墙。<br />
 **-b backlog :** 指定服务连接队列大小。默认为：1024，不明白不要动。<br />
 **-d :** 以守护进程的方式启动服务。默认为：前台模式。

**然后开始吧。**
**如：**curl http://127.0.0.1:18989/
