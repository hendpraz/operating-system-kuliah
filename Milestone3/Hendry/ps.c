#include "proc.h"

void main() {
    int i;
    int status;
    int succ;
    char curdir;
    char argc;
    char buff[2];
    char argv[4][16];
    
    enableInterrupts();
    //getCurdir
    interrupt(0x21, 0x21, &curdir, 0, 0);
    //getArgc
    interrupt(0x21, 0x22, &argc, 0, 0);
    //getArgv
    for (i = 0; i < argc; i++) {
		interrupt(0x21, 0x23, i, argv[i], 0);
	}
    buff[1] = '\0';
    for (i = 0; i < MAX_SEGMENTS; i++) {
        int m;
        interrupt(0x21, (i << 8) | 0x93, &m, &status, 0);
        if (m == SEGMENT_USED) {
			interrupt(0x21, 0x0, "PID: ", 0, 0);
            interrupt(0x21, 0x0, "PID: ", 0, 0);
            buff[0] = '0' + i;
            interrupt(0x21, 0x0, buff);
            switch (status) {
                case DEFUNCT: //0
                    interrupt(0x21, 0x0, " DEFUNCT", 0, 0);
                    break;
                case RUNNING: //1
                    interrupt(0x21, 0x0, " RUNNING", 0, 0);
                    break;
                case STARTING: //2
                    interrupt(0x21, 0x0, " STARTING", 0 ,0);
                    break;
                case READY: //3
                    interrupt(0x21, 0x0, " READY", 0, 0);
                    break;
                case PAUSED: //4
                    interrupt(0x21, 0x0, " PAUSED", 0, 0);
                    break;
                default:
                    interrupt(0x21, 0x0, " UNKNOWN STATE", 0, 0);
            }
            interrupt(0x21, 0x0, "\n", 0, 0);
        }
    }
    interrupt(0x21, (0x00 << 8) | 0x07, &succ, 0, 0);
}
