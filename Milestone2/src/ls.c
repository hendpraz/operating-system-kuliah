#include "func.h"

extern int interrupt(int num, int ax, int bx, int cx, int dx); 

struct dfEntry
{
	char parent;
	char filename[MAX_FILENAME];
};

int main() {
	char CURRENT_DIR;
	int i, j;
	char buf[SECTOR_SIZE];
	struct dfEntry entries[MAX_FILES];
	char test[2];

	test[1] = '\0';

	interrupt(0x21, 0x21, &CURRENT_DIR);
	interrupt(0x21, 0x02, entries, FILES_SECTOR, 0);

	printString("List File:\r\n");
	for (j = 0; j < MAX_FILES; j +=1 ) 
	{
		if (entries[j].filename[0] != '\0' &&  entries[j].parent == CURRENT_DIR ) 
		{
			printString(entries[j].filename); printString("\r\n");		
		}
	}

	interrupt(0x21, 0x02, entries, DIRS_SECTOR, 0);
	printString("List Directory:\r\n");
	for (j = 0; j < MAX_FILES; j+=1 ) 
	{
		if (entries[j].filename[0] != '\0' && entries[j].parent == CURRENT_DIR ) 
		{
			printString(entries[j].filename); printString("\r\n");		
		}
	}

	test[0] = CURRENT_DIR + '0';
	printString("Kamu Ada Di "); printString(test); printString(" directory"); printString("\r\n");

	// Terminasi
	interrupt(0x21, 0x07, &j);
}
