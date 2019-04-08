#include "func.h"

extern int interrupt(int num, int ax, int bx, int cx, int dx); // sudah didefinisikan di lib.asm

int main() {
  char sukses;
	char check;
	char input[256];//parameter input


	interrupt(0x21, 0x22, &check, 0, 0);

	if (check != 0) {
    interrupt(0x21, 0x23, 0, input, 0);
		writeln(input);
	} else {
		writeln("Error : Tidak menerima parameter");
	}

	interrupt(0x21, 0x07, &sukses, 0, 0);
}
