#include <stdlib.h>

#include "logger.h"

int
main(void)
{
    int i;

    setloggertype(LOGGER_STDOUT);
    setloggersev(LOGSEV_INFO);

    for (i = 0; i < 3; ++i)
    {
        setloggertime(i);
        logmsg(LOGSEV_INFO, "Testing level %d timestamps on logs", i);
    }

    exit(0);
}
