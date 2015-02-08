/* Copy me if you can.
 * by 20h
 * http://r-36.net
 */

#ifndef __ARG_H__
#define __ARG_H__

#include <stdlib.h>
char *argv0;

#define USED(x)        ((void)(x))

/* use main(int argc, char *argv[]) */
#define ARGBEGIN    /*@ignore@*/for (argv0 = *argv, argv++, argc--;\
                    argv[0] && argv[0][1]\
                    && argv[0][0] == '-';\
                    argc--, argv++) {\
                char _argc;\
                char **_argv;\
                int brk;\
                if (argv[0][1] == '-' && argv[0][2] == '\0') {\
                    argv++;\
                    argc--;\
                    break;\
                }\
                for (brk = 0, argv[0]++, _argv = argv;\
                        argv[0][0] && !brk;\
                        argv[0]++) {\
                    if (_argv != argv)\
                        break;\
                    _argc = argv[0][0];\
                    switch (_argc)

#define ARGEND            }\
                USED(_argc);\
            }\
            USED(argv);\
            USED(argc); /*@end@*/

#define ARGC()        _argc

#define EARGF(x)    ((argv[0][1] == '\0' && argv[1] == NULL)?\
                ((x), abort(), (char *)0) :\
                (brk = 1, (argv[0][1] != '\0')?\
                    (&argv[0][1]) :\
                    (argc--, argv++, argv[0])))

#define ARGF()        ((argv[0][1] == '\0' && argv[1] == NULL)?\
                (char *)0 :\
                (brk = 1, (argv[0][1] != '\0')?\
                    (&argv[0][1]) :\
                    (argc--, argv++, argv[0])))

#endif
