#include "func.h"

#define NEW_DIR_CREATED 0

extern int interrupt(int num, int ax, int bx, int cx, int dx);  // sudah didefinisikan di lib.asm

int main() {
	char check;
	char dir;
	char input[256];
	char currentdir;

	interrupt(0x21, 0x21, &currentdir);
	interrupt(0x21, 0x22, &check);

	if (check != 0) {
		interrupt(0x21, 0x23, 0, input, 0);
		interrupt(0x21, (currentdir<<8)|0x08, input, &dir);
		if (dir == NEW_DIR_CREATED) {
			writeln("Direktori baru berhasil dibuat");
		} else {
			writeln("Error : Gagal membuat direktori baru");
		}
	} else {
		writeln("Error : tidak ada parameter");
	}

	interrupt(0x21, 0x07, &dir);
}
