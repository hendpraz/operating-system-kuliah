void main() {
    char curdir;
    char argc;   
    char argv[4][32];
    int i;

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
		for (i = 0; i < argc; i++) {
			//printString semua argumen
			interrupt(0x21, 0x0, argv[i], 0, 0);
			interrupt(0x21, 0x0, " ", 0, 0);
		}
		//printString newline di akhir baris
        interrupt(0x21, 0x0, "\n", 0, 0);
    } else if(argc == 0){
		//printString
		interrupt(0x21, 0x0, "Cara penggunaan : tulis 'echo <sesuatu>'", 0, 0);
		interrupt(0x21, 0x0, "\n", 0, 0);
	}
    //terminateProgram(0) BX = 0 AL = 0x07;
    interrupt(0x21,0x07,0,0,0);
}
