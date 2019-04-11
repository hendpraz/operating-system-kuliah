#include "func.h"

#define NOT_FOUND -1

extern int interrupt(int num, int ax, int bx, int cx, int dx);

//Function needed
 
//void printString
//void readString(char *)
//int strcmp
//void writeln

int main() {
	//Kamus
	char currentDir;
	char argc;
	char argv[256], argw[15];
	int idx = 0;
	int count = 0;
	int success;
	int countSector; 
	char buffer[512], inp[512], otp[2048];
	
	//Algoritma
	interrupt(0x21, 0x21, &currentDir);
	interrupt(0x21, 0x22, &argc);

	writeln("Menjalankan cat...");

	//Mengabaikan argumen selain 2 argumen pertama
	if (argc > 2) {
		argc = 2;
	}

	//Argumen hanya 1, mencetak isi file
	if (argc == 1) {
		interrupt(0x21, 0x23, 0, argv, 0);
		interrupt(0x21, (currentDir << 8) | 0x04, buffer, argv, &success);

		printString(argv);
		printString("\r\n");

		if (success == NOT_FOUND) {
			writeln("Error : File tidak ditemukan");
		} else { //success = SUCCESS;
			writeln("File ditemukan");
			writeln("Isi file : ");
			writeln(buffer);
		}
	} else if (argc == 2) {
		interrupt(0x21, 0x23, 0, argv, 0);
		interrupt(0x21, (currentDir << 8) | 0x04, buffer, argv, &success);

		printString(argv);
		printString("\r\n");

		if (success == NOT_FOUND) {
			interrupt(0x21, 0x23, 1, argw, 0);
			if (strcmp(argw, "-w", 2)) {
				writeln("Masukkan isi file :");
				readStringFull(inp);
				
				while (inp[idx] != '\0') {
					idx = 0;
					otp[count] = inp[idx];
					idx++;
					count++;
				}
				
				interrupt(0x21, (currentDir << 8) | 0x05, otp, argv, &countSector, currentDir);

				if (countSector != NOT_FOUND) {
					writeln("\n\r ----File tersimpan!----");
				}
			} else{
				writeln("Error : Command tidak tersedia!");
				writeln("Command tersedia : -w");
			}
		} else {
			writeln("Error : File sudah ada!");
		}
	} else {
		writeln("Error : Tidak ada parameter");
		writeln("Cara penggunaan, tulis 'cat <namafile> <command>'");
	}

	interrupt(0x21, 0x07, &success);
}
