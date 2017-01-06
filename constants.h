//
// Created by cora32 on 02.01.2017.
//

#define USAGE "Usage: \n"\
                "\tiotminion <ip> <port> <threads> <timeout>\n"\
                "\tiotminion -f <filename> <threads> <timeout>\n"

#define DEFAULT_TIMEOUT 3000L //timeout in milliseconds
#define DEFAULT_THREACOUNT 1
#define DEFAULT_DISABLE_SHUFFLE false

#define OPT_HELP_FULL "help"
#define OPT_HELP_SHORT "h"
#define OPT_IP_FULL "ip"
#define OPT_IP_SHORT "i"
#define OPT_PORT_FULL "port"
#define OPT_PORT_SHORT "p"
#define OPT_FILE_FULL "file"
#define OPT_FILE_SHORT "f"
#define OPT_THREADS_FULL "threads"
#define OPT_THREADS_SHORT "t"
#define OPT_TIMEOUT_FULL "timeout"
#define OPT_TIMEOUT_SHORT "T"
#define OPT_SHUFFLE_FULL "disable-shuffle"
#define OPT_SHUFFLE_SHORT "s"