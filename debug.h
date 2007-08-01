/* $Id: debug.h 339 2007-03-17 02:12:24Z msoulier $
 */
/* A simple software assertion that does not terminate the executable.
 * 0 is good, 1 is bad.
 */
#define swerr(f) ((f) ? 0 : \
    fprintf(stderr, "libesdb SWERR: File -> %s, Line -> %d\n", __FILE__, __LINE__) > 0)

#ifdef DEBUG
#define debug(args) (printf("DEBUG: "), printf args)
#else
#define debug(args)
#endif
