#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "mikelib.h"

int
main(int argc, char *argv[])
{
    if (argc < 2)
        setloggertime(LOGTIME_NONE);
    else
        setloggertime(atoi(argv[1]));
    setloggertype(LOGGER_STDOUT, NULL);
    setloggersev(LOGSEV_INFO);
    while (1)
    {
        logmsg(LOGSEV_INFO, "Here I am again");
        sleep(1);
        fflush(stdout);
    }
    exit(0);
}
