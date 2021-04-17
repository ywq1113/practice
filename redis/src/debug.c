#include "./server.h"
//#include "sha1.h"   /* SHA1 is used for DEBUG DIGEST */
//#include "crc64.h"
//#include "bio.h"

#include <arpa/inet.h>
#include <signal.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

/* Globals */
static int bug_report_start = 0; /* True if bug report header was already logged. */
static pthread_mutex_t bug_report_start_mutex = PTHREAD_MUTEX_INITIALIZER;

/* forward declaration */
void bugReportStart(void);
void bugReportEnd(int killViaSignal, int sig);


/*
 * @brief 将要打印的信息 msg 存入 file 中第 line 行
 * @param file
 * @param line
 * @param msg
 * @return void
 */
void __serverPanic(const char* file, int line, const char *msg, ...) {
    /*
     * 变参数用法
     * va_list 是指向参数的指针
     * va_start 宏初始化刚定义的 va_list 变量
     * va_arg 返回可变的参数，第二个参数是你要返回的参数的类型
     * va_end 结束可变参数的获取
     */
    va_list ap;
    va_start(ap, msg);
    char fmtmsg[256];
    vsnprintf(fmtmsg, sizeof fmtmsg, msg, ap);
    va_end(ap);

    bugReportStart();
    serverLog(LL_WARNING,"------------------------------------------------");
    serverLog(LL_WARNING,"!!! Software Failure. Press left mouse button to continue");
    serverLog(LL_WARNING,"Guru Meditation: %s #%s:%d",fmtmsg,file,line);

    bugReportEnd(0, 0);
}

void bugReportStart() {
    pthread_mutex_lock(&bug_report_start_mutex);
    if (bug_report_start == 0) {
        serverLogRaw(LL_WARNING|LL_RAW, 
        "\n\n === REDIS BUG REPORT START: Cut & paste starting from here ===\n");
        bug_report_start = 1;
    }
    pthread_mutex_unlock(&bug_report_start_mutex);
}

void bugReportEnd(int killViaSignal, int sig) {
    /*
     * 捕获信号流程
     * https://man7.org/linux/man-pages/man2/sigaction.2.html
     * 
     * SA_NODEFER: 
     * SA_ONSTACK: 在备用信号堆栈上调用信号处理程序
     * SA_RESETHAND: 进入信号处理程序后，将信号操作恢复为默认值（此前用SA_ONESHOT）
     * 
     * sigaction(...): 更改进程在收到特定信号后采取的操作
     */
    struct sigaction act;



    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;  /* SIG_DFL 默认信号处理，SIG_IGN 忽略信号 */
    sigaction(sig, &act, NULL);
    kill(getpid(), sig);
}