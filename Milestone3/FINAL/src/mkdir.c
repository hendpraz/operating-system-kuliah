char compare(char * arr1, char * arr2, int length);

#define MAX_FILENAME 15
#define ALREADY_EXISTS -2
#define NOT_FOUND -1
#define SUCCESS 0

void main() {
	int i;
    int succ;
    char curdir;
    char argc;
    char dest;
    char argv[4][32];
    char buff[MAX_FILENAME + 1];
    char mode = 0;

    enableInterrupts();
    //getCurdir
    interrupt(0x21, 0x21, &curdir, 0, 0);
    //getArgc
    interrupt(0x21, 0x22, &argc, 0, 0);
    for (i = 0; i < argc; i++) {
		//getArgv
		interrupt(0x21, 0x23, i, argv[i], 0);
	}
    if (argc > 0) {
		//makeDirectory
        interrupt(0x21, (curdir << 8) | 0x08, argv[0], &succ, 0);
        if (succ == ALREADY_EXISTS) {
			interrupt(0x21, 0x0, "Directory telah ada! ", 0, 0);
			interrupt(0x21, 0x0, argv[0], 0, 0);
			interrupt(0x21, 0x0, "\n", 0, 0);
		} else if (succ == NOT_FOUND) {
			interrupt(0x21, 0x0, "Directory tidak ditemukan! ", 0, 0);
			interrupt(0x21, 0x0, argv[0], 0, 0);
			interrupt(0x21, 0x0, "\n", 0, 0);
		} else if (succ == SUCCESS){
			interrupt(0x21, 0x0, "Directory berhasil dibuat : ", 0, 0);
			interrupt(0x21, 0x0, argv[0], 0, 0);
			interrupt(0x21, 0x0, "\n", 0, 0);
		}
    }
    //terminateProgram(0) BX = 0 AL = 0x07;
    interrupt(0x21,0x07,0,0,0);
}
