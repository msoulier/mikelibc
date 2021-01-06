#include <stdlib.h>

#include "mlogger.h"

#define LOGGER_TIMES 4

int
main(void)
{
    int i;
    char buffer[1024];

    setloggertype(LOGGER_STDOUT, NULL);
    setloggersev(LOGSEV_INFO);

    for (i = 0; i < LOGGER_TIMES+1; ++i)
    {
        setloggertime(i);
        logmsg(LOGSEV_INFO, "Testing level %d timestamps on logs", i);
    }

    setloggertype(LOGGER_FILE, "./mylogfile");

    for (i = 0; i < LOGGER_TIMES+1; ++i)
    {
        setloggertime(i);
        logmsg(LOGSEV_INFO, "Testing level %d timestamps on logs", i);
    }

    gettimestamp(FORMAT_TAI64N, buffer);
    printf("\nCurrent tai64n timestamp is %s\n", buffer);

    exit(0);
}
