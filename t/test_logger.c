#include <stdlib.h>

#include "logger.h"

int
main(void)
{
    int i;
    char buffer[1024];

    setloggertype(LOGGER_STDOUT, NULL);
    setloggersev(LOGSEV_INFO);

    for (i = 0; i < 4; ++i)
    {
        setloggertime(i);
        logmsg(LOGSEV_INFO, "Testing level %d timestamps on logs", i);
    }

    setloggertype(LOGGER_FILE, "./mylogfile");

    for (i = 0; i < 4; ++i)
    {
        setloggertime(i);
        logmsg(LOGSEV_INFO, "Testing level %d timestamps on logs", i);
    }

    sprintf(buffer, "cat %s", "./mylogfile");
    printf("\nOutput of file log is:\n");
    system(buffer);

    exit(0);
}
