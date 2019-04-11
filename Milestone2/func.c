#include "func.h"

void readStringFull(char *string){
	int inp;
	int countInp = 0; //banyaknya input
	while(1){
		inp = interrupt(0x16,0,0,0,0);
		if (inp == '\r'){              
			string[countInp] = 0x0;
			string[countInp+1] = '\r';
			string[countInp+2] = '\n';
			return;
		}
		else if (inp == '\b'){
			if (countInp >= 1){
				string[countInp] = 0x0;
				interrupt(0x10,0xE00+'\b',0,0,0);
				interrupt(0x10,0xE00+'\0',0,0,0);
				interrupt(0x10,0xE00+'\b',0,0,0);
				countInp--;
			}
		}
		else{
			string[countInp] = inp;
			interrupt(0x10, 0xE00+inp, 0, 0, 0);
			countInp++;
		}     
	}
}

void writeln(char *string){
	//printString(string)
	interrupt(0x21, 0x0, string, 0, 0);
	interrupt(0x21, 0x0, "\n\r", 0, 0);
}

int strcmp(char *s1, char *s2, int length){
	int idx = 0;
	//cek kesamaan s1 dan s2
	while(idx < length){
		if (s1[idx] == '\0' || s2[idx] == '\0') {
			return 0;
		}
		if (s1[idx] != s2[idx]) {
			return 0;
		}
		idx++;
	}
	return TRUE;
}

void printString(char *string) {
    interrupt(0x21, 0x0, string, 0, 0);
}

void readString(char *string) {
    interrupt(0x21, 0x1, string, 0, 0);
}

void readSector(char *buffer, int sector) {
    interrupt(0x21, 0x2, buffer, sector, 0);
}

void writeSector(char *buffer, int sector) {
    interrupt(0x21, 0x3, buffer, sector, 0);
}

void readFile(char *buffer, char *filename, int *success, char parentIndex) {
    interrupt(0x21, parentIndex << 8 | 0x4, buffer, filename, success);
}

void writeFile(char *buffer, char *filename, int *sectors, char parentIndex) {
    interrupt(0x21, parentIndex << 8 | 0x5, buffer, filename, sectors);
}

void deleteFile(char *path, int *result, char parentIndex){
    interrupt(0x21, parentIndex << 8 | 0x9, path, result, 0);
}

void deleteDirectory(char *path, int *result, char parentIndex){
    interrupt(0x21, parentIndex << 8 | 0xA, path, result, 0);
}

void executeProgram(char *filename, int segment, int *success, char p) {
    interrupt(0x21, p << 8 | 0x6, filename, segment, success);
}

void terminateProgram(int *result) {
    interrupt(0x21, 0x7, result, 0, 0);
}

void makeDirectory(char *path, int *result, char parentIndex) {
    interrupt(0x21, parentIndex << 8 | 0x8, path, result, 0);
}

void putArgs(char curdir, char argc, char **argv) {
    interrupt(0x21, 0x20, curdir, argc, argv);
}

void getCurdir(char *curdir) {
    interrupt(0x21, 0x21, curdir, 0, 0);
}

void getArgc(char *argc) {
    interrupt(0x21, 0x22, argc, 0, 0);
}

void getArgv(char index, char *argv) {
    interrupt(0x21, 0x23, index, argv, 0);
}

void println(char *s) {
    printString(s);
    printString("\r\n");
}

void findDirectory(char *path, char *result, char parentIndex, int *attempt) {
    char dir[SECTOR_SIZE];
    char name[MAX_FILENAME];
    int equal = TRUE;
    int i,j;
    
    readSector(dir, DIR_SECTOR);
    
    // String kosong
    if (path[0] == 0){
        *attempt = NOT_FOUND;
        return;
    }

    // Cek direktori pertama
    i = 0;
    while ((path[i] != 0) && (path[i] != '/')){
        name[i] = path[i];
        i++;
    }
    name[i] = 0;

    // Cek direktori di parentIndex
    if (path[i] == '/') {
        findDirectory(name, result, parentIndex, attempt);
        if (attempt == NOT_FOUND)
            return;
        parentIndex = *result;
        findDirectory(path + i + 1, result, parentIndex, attempt);
    } else{
        // Cek di parentIndex
        for (i = 0; i < MAX_FILES; i++){
            if (dir[i * DIR_ENTRY_LENGTH] != parentIndex)
                continue;
            equal = TRUE;
            for (j = 0; (j < MAX_FILENAME) && (equal) && ((path[j] != 0) || (dir[i * DIR_ENTRY_LENGTH + 1 + j])); j++)
                equal = equal && (path[j] == dir[i * DIR_ENTRY_LENGTH + 1 + j]);
            if (equal == TRUE){
                *result = i;
                *attempt = 0;
                return;
            }
        }
        *attempt = NOT_FOUND;
    }
}

void findFile(char *path, char *result, char parentIndex, int *attempt) {
    char files[SECTOR_SIZE];
    char name[MAX_FILENAME];
    int i,j;
    int equal = TRUE;

    readSector(files, FILE_SECTOR);

    // String path kosong
    if (path[0] == 0){
        *attempt = NOT_FOUND;
        return;
    }

    // Cek file yang ada di parent
    for (i = 0; i < MAX_FILES; i++){
        if (files[i * DIR_ENTRY_LENGTH + 1] == 0)
            continue;

        // Cek parentIndex
        if (files[i * DIR_ENTRY_LENGTH] != parentIndex)
            continue;
        equal = TRUE;
        for (j = 0; (j < MAX_FILENAME) && (equal) && ((path[j] != 0) || (files[i * DIR_ENTRY_LENGTH + 1 + j] != 0)); j++)
            equal = equal && (path[j] == files[i * DIR_ENTRY_LENGTH + 1 + j]);
        
        // Found
        if (equal == TRUE){
            *result = i;
            *attempt = 0;
            return;
        }
    }
    *attempt = NOT_FOUND;
}

void printDirectory(char curdir){
    char dir[SECTOR_SIZE];
    int i, attempt;
    char j, result;
    char path[MAX_FILENAME];
    int found;

    if (curdir == 0xFF){
        printString("@root:~");
    } else{
        readSector(dir, DIR_SECTOR);
        for (i = 0; i < MAX_FILENAME; i++){
            if (dir[curdir * DIR_ENTRY_LENGTH + 1 + i] != '\0'){
                path[i] = dir[curdir * DIR_ENTRY_LENGTH + 1 + i];
            } else{
                break;
            }
        }
        found = FALSE;
        for (j = 0x00; j < 0x1F; j++){
            // printString(path);
            findDirectory(path, &result, j, &attempt);
            if (attempt == 0){
                printDirectory(j);
                found = TRUE;
                break;
            }
        }
        
        if (!found){
            printDirectory(0xFF);
        }

        // printDirectory(0xFF);
        interrupt(0x10, 0xE00+'/', 0, 0, 0);
        for (i = 0; i < MAX_FILENAME; i++){
            if (dir[curdir * DIR_ENTRY_LENGTH + 1 + i] != 0){
                interrupt(0x10, 0xE00+dir[curdir * DIR_ENTRY_LENGTH + 1 + i], 0, 0, 0);
            } else{
                break;
            }
        }
    }
}
