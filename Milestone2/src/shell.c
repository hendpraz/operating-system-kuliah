/*
	PROGRAM SHELL
*/

#include "func.h"

extern int interrupt(int num, int ax, int bx, int cx, int dx); 
void splitCommand(char *Argv[], char *BUFFER, char* Argc) ;

int main() {

	///ALL Variabel
	char BUFFER[512]; 
	int success;
	char* Argv[64];
	char Argc;
	char CURRENT_DIR, USE_DIR;

	interrupt(0x21, 0x21, &CURRENT_DIR);
	
	Argc = 0;
	printString("\r\n$ ");
	
	// Membaca BUFFER
	readString(BUFFER);
	printString("\r\n");
	
	// Memisahkan Command
	splitCommand(Argv, BUFFER, &Argc);
	Argc--;

	if (BUFFER[0] != '\0') 
	{
		interrupt(0x21, 0x20, CURRENT_DIR, Argc, Argv);
		interrupt(0x21, 0xFF << 8 | 0x06, BUFFER, 0x2000, &success);		
	}
	else if (strcmp(BUFFER, "cd", 2)) 
	{
		if (Argc != 0) 
		{
			if (Argv[0][0] != '/') 
			{
				USE_DIR = CURRENT_DIR;
			}
			else 
			{
				USE_DIR = 0xFF;
			} 		
			interrupt(0x21, 0x30, Argv[0], &success, &USE_DIR);
			
			// Pengecekan Berhasil Atau Tidak
			if (success != SUCCESS) 
			{ 
				printString("ERROR : Tidak Dapat Berpindah Ke DIRECTORY PATH\r\n");  // Tidak Berhasil
			}
			else
			{
				printString("Sekarang Kamu Ada Di \""); printString(Argv[0]); printString("\" directoryY\r\n");
				CURRENT_DIR = USE_DIR;
				interrupt(0x21, 0x20, CURRENT_DIR);
			} 
			 
			
		}
		else 
		{
			USE_DIR = 0xFF;
			CURRENT_DIR = USE_DIR;
			printString("Sekarang Kamu Ada Di Akar\r\n");
			interrupt(0x21, 0x20, CURRENT_DIR);
		} 
		
		
	} 

	// Terminasi
	interrupt(0x21, 0x07, &success);
}

// Memisahkan Command
void splitCommand(char *Argv[], char *BUFFER, char* Argc) 
{

	while (*BUFFER != '\0') 
	{
		while (*BUFFER == ' ') 
		{
			*BUFFER++ = '\0';
		}
		
		// MELIHAT ARG PERTAMA
		if (*Argc > 0) 
		{
			Argv[*Argc - 1] = BUFFER;
		}
		
		while (*BUFFER != '\0') 
		{
			if (*BUFFER == ' ')
				break;
			BUFFER++;
		}
		
		(*Argc)++;
	}
}
