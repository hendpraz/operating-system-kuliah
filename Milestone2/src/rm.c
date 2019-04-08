#include "func.h"

extern int interrupt(int num, int ax, int bx, int cx, int dx);// sudah didefinisikan di lib.asm

int main() {
	char input[32];
	char check;
	int i;
	int dirDel, fileDel;
	char curdir;

	interrupt(0x21, 0x21, &curdir);
	interrupt(0x21, 0x22, &check);

	if (check != 0) {
		for (i = 0; i < check; i++) {
			interrupt(0x21, 0x23, i, input);
			writeln(input);
			interrupt(0x21, (curdir << 8) | 0x09, input, &fileDel);
			interrupt(0x21, (curdir << 8) | 0x0A, input, &dirDel);

			if (dirDel == SUCCESS || fileDel) {
				writeln(input);
				writeln("Berhasil");
			} else {
				writeln(input);
				writeln(" Penghapusan gagal!");
			}
		}
	} else {
		writeln("Error : Tidak ada argumen untuk menghapus direktori, silahkan input filename dengan path");
	}
	interrupt(0x21, 0x7, &i);
}
