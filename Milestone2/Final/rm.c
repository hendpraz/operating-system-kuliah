#define SUCCESS 0

extern int interrupt(int num, int ax, int bx, int cx, int dx);

void printString(char *);
void readString(char *);

int main() {
	int dirSuccess, fileSuccess;
	int i;
	char curdir;
	char argv[32];
	char argc;

	interrupt(0x21, 0x21, &curdir);
	interrupt(0x21, 0x22, &argc);

	if (argc == 0) {
		printString("ERROR : No argument to remove directory, expected filename with path\r\n");
	} else {
		for (i = 0; i < argc; i++) {
			interrupt(0x21, 0x23, i, argv);
			printString(argv);
			printString("\r\n");
			interrupt(0x21, (curdir << 8) | 0x0A, argv, &dirSuccess);
			interrupt(0x21, (curdir << 8) | 0x09, argv, &fileSuccess);

			if (dirSuccess == SUCCESS || fileSuccess) {
				printString(argv);
				printString("SUCCESS!\r\n");
			} else {
				printString(argv);
				printString(" doesn't successfully deleted!\r\n");
			}
		}	
	}
	interrupt(0x21, 0x7, &i);
}

void printString(char *str) {
	interrupt(0x21, 0, str);
}

void readString(char *str) {
	interrupt(0x21, 1, str);
}