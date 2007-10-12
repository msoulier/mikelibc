#include <stdlib.h>

#include "logger.h"

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

    sprintf(buffer, "cat %s", "./mylogfile");
    printf("\nOutput of file log is:\n");
    system(buffer);

    exit(0);
}
