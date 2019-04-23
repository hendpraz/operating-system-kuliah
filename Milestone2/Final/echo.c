extern int interrupt(int num, int ax, int bx, int cx, int dx); // defined in lib.asm

void printString(char *);
void readString(char *);

int main() {
	// Variable
	char argc;
	char argv[256], success;

	interrupt(0x21, 0x22, &argc, 0, 0);

	if (argc > 1) {
		argc = 1;
	}

	if (argc == 0) {
		printString("ERROR : No paramater for echo command");
	} else if (argc == 1) {
		interrupt(0x21, 0x23, 0, argv, 0);
		printString(argv);
		printString("\r\n");
	}

	interrupt(0x21, 0x07, &success, 0, 0);
}

void printString(char *str) {
	interrupt(0x21, 0, str);
}

void readString(char *str) {
	interrupt(0x21, 1, str);
}
