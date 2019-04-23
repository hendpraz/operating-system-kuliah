#define MAIN
#include "proc.h"

#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 32
#define MAX_DIRS 32
#define MAX_FILENAME 15
#define ENTRY_LENGTH 16
#define MAX_SECTORS 16
#define MAP_SECTOR 0x100
#define DIRS_SECTOR 0x101
#define FILES_SECTOR 0x102
#define SECTORS_SECTOR 0x103
#define ARGS_SECTOR 512
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define INSUFFICIENT_ENTRIES -3
#define INSUFFICIENT_SEGMENTS -2
#define ALREADY_EXISTS -2
#define EMPTY 0x00
#define USED 0xFF
#define DIR_ROOT 0xFF
#define NOT_FOUND -1
#define SUCCESS 0

// Fungsi awal
// #F01
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string, int disableProcessControls); //perubahan
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void clear(char *buffer, int length);

// Fungsi pengelola file
// #F02
void findFile(char * parent, char * current, char * filename, int * idx, int * result);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);
void deleteFileIndex(char current);

// Fungsi pengelola direktori
// #F03
void findDir(char * parent, char * current, char * filename, int * idx, int * result);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex);
void deleteDirectoryIndex(char current);

// Fungsi eksekusi dan terminasi
// #F04
void executeProgram (char *path, int *result, char parentIndex, char async);
void executeProgramAsync (char *path, int *result, char parentIndex);
void terminateProgram (int *result);

// Fungsi mengolah argumen
// #F05
void putArgs (char curdir, char argc, char **argv);
void getCurdir (char *curdir);
void getArgc (char *argc);
void getArgv (char index, char *argv);

//Fungsi milestone 3
// #F06
void yieldControl ();
void sleep ();
void pauseProcess (int segment, int *result);
void resumeProcess (int segment, int *result);
void killProcess (int segment, int *result);

// Fungsi tambahan
// F07
void writeln(char *string);
char compare(char * arr1, char * arr2, int length);

int main() 
{
	//KAMU
    int succ = 0;
    char buffer[MAX_SECTORS * SECTOR_SIZE];
    int x;
    int y;
    char *argv[2];
    char buff[21];
    
    //AGLROITMA
    initializeProcStructures();
    makeInterrupt21();
    makeTimerInterrupt();
    //Membersihkan layar
    y = 0;
    while (y < 30) {
        x = 0;
        while (x < 80) {
            putInMemory(0xB000, 0x8000 + (80 * y + x) * 2, '\0');
            putInMemory(0xB000, 0x8001 + (80 * y + x) * 2, 0xF);
            x++;
        }
        y++;
    }
    interrupt(0x10, 0x200, 0, 0, 0);
    // Set argumen default pada sektor argumen
    interrupt(0x21, 0x20, 0xFF, 0, argv);
    // Memanggil shell
    interrupt(0x21, 0xFF << 8 | 0x6, "shell", &succ);
    while (1);  
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) 
{
    char AL, AH;
    int i = 0;
    int j = 0;
    int * p;
    char d;
    AL = (char) (AX);
    AH = (char) (AX >> 8);
    switch (AL) {
        case 0x00:
            printString(BX);
            break;
        case 0x01:
            readString(BX, CX);
            break;
        case 0x02:
            readSector(BX, CX);
            break;
        case 0x03:
            writeSector(BX, CX);
            break;
        case 0x04:
            readFile(BX, CX, DX, AH);
            break;
        case 0x05:
            writeFile(BX, CX, DX, AH);
            break;
        case 0x06:
            executeProgram(BX, CX, AH, 0);
            break;
        case 0x07:
            terminateProgram(BX);
            break;
        case 0x08:
            makeDirectory(BX, CX, AH);
            break;
        case 0x09:
            deleteFile(BX, CX, AH);
            break;
        case 0x0A:
            deleteDirectory(BX, CX, AH);
            break;
        case 0x20:
            putArgs(BX, CX, DX);
            break;
        case 0x21:
            getCurdir(BX);
            break;
        case 0x22:
            getArgc(BX);
            break;
        case 0x23:
            getArgv(BX, CX);
            break;
        case 0x30:
            yieldControl();
            break;
        case 0x31:
            sleep();
            break;
        case 0x32:
            pauseProcess (BX, CX);
            break;
        case 0x33:
            resumeProcess (BX, CX);
            break;
        case 0x34:
            killProcess (BX, CX);
            break;
        case 0X90:
            findDir(&AH, &d, BX, &i, CX);
            p = (int *) CX;
            if (*p == SUCCESS) {
                *p = 1;
            } else {
                *p = 0;
            }
            p = (int *) DX;
            *p = d;
            break;
        case 0X91:
            findFile(&AH, &d, BX, &i, CX);
            p = (int *) CX;
            if (*p == SUCCESS) {
                *p = 1;
            } else {
                *p = 0;
            }
            p = (int *) DX;
            *p = d;
            break;
        case 0x92:
            executeProgram(BX, CX, AH, 1);
            break;
        case 0x93:
            setKernelDataSegment();
            i = memoryMap[AH];
            restoreDataSegment();
            if (i == SEGMENT_USED) {
                int seg = (AH + 2) << 12;
                setKernelDataSegment();
                j = getPCBOfSegment(seg)->state;
                restoreDataSegment();
            } else {
                j = -1;
            }
            *((int *) BX) = i;
            *((int *) CX) = j;
            break;
        default:
            printString("Invalid interrupt");
    }
}

void printString(char *string) 
{
    while (*string != '\0') {
        if ((*string) == '\r') {
            interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
        } else if ((*string) == '\n') {
            interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
        }
        interrupt(0x10, 0xE00 + (*string), 0, 0, 0);
        string++;
    }
}

void readString(char *string, int disableProcessControls) 
{
    char reading = TRUE;
    int count = 0;
    while (reading) {
        char c = interrupt(0x16, 0, 0, 0, 0);
        if ((c == 3) && (!disableProcessControls)) {
            char success;
            interrupt(0x10, 0xE00 + '^', 0, 0, 0);
            interrupt(0x10, 0xE00 + 'C', 0, 0, 0);
            terminateProgram(&success);
        } else if ((c == 26) && (!disableProcessControls)) {
            char success;
            int parentSegment;
            interrupt(0x10, 0xE00 + '^', 0, 0, 0);
            interrupt(0x10, 0xE00 + 'Z', 0, 0, 0);
            setKernelDataSegment();
            parentSegment = running->parentSegment;
            restoreDataSegment();
            resumeProcess(parentSegment, &success);
			if (success == SUCCESS) {
				sleep();
			}
        } else if (c == '\r') { // Return/Enter
            interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
            interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
            (*string) = '\0';
            reading = FALSE;
        } else if (c == '\b') {
            if (count > 0) {
                interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
                interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
                interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
                (*string) = '\0';
                string--;
                count--;
            }
        } else if (c >= 32) {
            interrupt(0x10, 0xE00 + c, 0, 0, 0);
            (*string) = c;
            string++;
            count++;
        
        }
    }
}

int mod(int a, int b) 
{
    while(a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b) 
{
    int q = 0;
    while(q*b <=a) {
        q = q+1;
    }
    return q-1;
}

void readSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void clear(char *buffer, int length)
{
    int i;
    for(i = 0; i < length; ++i){buffer[i] = EMPTY;}
}

// F02
void findFile(char * parent, char * current, char * filename, int * idx, int * result) 
{
    char name[MAX_FILENAME + 1];
    char dir[SECTOR_SIZE];
    char file;
    char found = FALSE;
    int j;
    int count;
    int i = 0;
    if (filename[*idx] == '/'){*idx++;}
    while ((filename[*idx + i] != '\0') && (filename[*idx + i] != '/')) {
        name[i] = filename[*idx + i];
        i++;
    }
    // end null terminator.
    file = filename[*idx + i] == '\0';
    name[i] = '\0';
    j = i;
    if (file) {
        readSector(dir, FILES_SECTOR);
        count = MAX_FILES;
    } else {
        readSector(dir, DIRS_SECTOR);
        count = MAX_DIRS;
    }
    // Find file
    i = 0;
    while ((!found) && (i < count)) {
        if ((dir[i * ENTRY_LENGTH] == *parent) && (compare(name, dir + (i * ENTRY_LENGTH) + 1, MAX_FILENAME))) {
            found = TRUE;
        } else {i++;}
    }
    if (found) {
        *current = i;
        if (file) {*result = SUCCESS;} // File found.
        else {// Recursively search
            *parent = *current;
            *idx = *idx + j + 1;
            findFile(parent, current, filename, idx, result);
        }
    } else {
        *result = NOT_FOUND;
        *current = *parent;
    }
}

void readFile(char *buffer, char *path, int *result, char parentIndex) 
{
    char parent = parentIndex;
    int i = 0;
    char current;
    char dir[SECTOR_SIZE];
    findFile(&parent, &current, path, &i, result);
    readSector(dir, SECTORS_SECTOR);

    if (*result == SUCCESS) {
        char copying = TRUE;
        char * sectors = dir + (current * ENTRY_LENGTH);
        i = 0;
        while ((copying) && (i < MAX_SECTORS)) {
            char sector = sectors[i];
            if (sector == 0) {copying = FALSE;} 
            else{readSector(buffer + i * SECTOR_SIZE, sector);}
            i++;
        }
        *result = current;
    }
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) 
{
    char map[SECTOR_SIZE];
    char sectorBuffer[SECTOR_SIZE];
    char sector[SECTOR_SIZE];
    char files[SECTOR_SIZE];
    int i, j, sectorCount;
    int dirIndex;

    readSector(map, MAP_SECTOR);
    readSector(files, FILES_SECTOR);
    readSector(sector, SECTORS_SECTOR);
    for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
        if (map[i] == EMPTY) {++sectorCount;}
    }
    if (sectorCount < *sectors) {*sectors = INSUFFICIENT_SECTORS;} 
    else {
        for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex) {
            if (files[dirIndex * ENTRY_LENGTH + 1] == '\0') {break;}
        }
        if (dirIndex < MAX_FILES) {
            char parent = parentIndex;
            char current;
            int result;
            j = 0;
            findFile(&parent, &current, path, &j, &result);
            if (result == SUCCESS) {
                *sectors = ALREADY_EXISTS;
            } else {
                char filename[MAX_FILENAME + 1];
                char offset = j;
                char file;
                while ((path[j] != '\0') && (path[j] != '/')) {
                    filename[j - offset] = path[j];
                    j++;
                }
                filename[j - offset] = '\0';
                file = path[j] == '\0';
                if (file) {
                    files[dirIndex * ENTRY_LENGTH] = parent;
                    j = 0;
                    while (filename[j] != '\0') {
                        files[dirIndex * ENTRY_LENGTH + 1 + j] = filename[j];
                        j++;
                    }
                    writeSector(files, FILES_SECTOR);
                    for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
                        if (map[i] == EMPTY) {
                            map[i] = USED;
                            sector[dirIndex * ENTRY_LENGTH + sectorCount] = i;
                            clear(sectorBuffer, SECTOR_SIZE);
                            for (j = 0; j < SECTOR_SIZE; ++j) {
                                sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j];
                            }
                            writeSector(sectorBuffer, i);
                            ++sectorCount;
                        }   	 
                    }
                    writeSector(map, MAP_SECTOR);
                    writeSector(sector, SECTORS_SECTOR);
                } else {*sectors = NOT_FOUND;}
            }
        } else {*sectors = INSUFFICIENT_ENTRIES;}
    }

}

void deleteFile(char *path, int *result, char parentIndex) 
{
    char parent = parentIndex;
    int i = 0;
    char current;
    findFile(&parent, &current, path, &i, result);
    if (*result == SUCCESS) {deleteFileIndex(current);}
}

void deleteFileIndex(char current) 
{
    char map[SECTOR_SIZE];
    char sectors[SECTOR_SIZE];
    char files[SECTOR_SIZE];    
    int j = 0;
    readSector(files, FILES_SECTOR);
    readSector(sectors, SECTORS_SECTOR);
    readSector(map, MAP_SECTOR);
    files[current * ENTRY_LENGTH] = 0x00;// null terminator.
    files[current * ENTRY_LENGTH + 1] = '\0';
    while ((sectors[current * ENTRY_LENGTH + j] != 0) && (j < MAX_SECTORS)) {
        map[sectors[current * ENTRY_LENGTH + j]] = EMPTY;
        sectors[current * ENTRY_LENGTH + j] = 0;
        j++;
    }
    writeSector(files, FILES_SECTOR);
    writeSector(sectors, SECTORS_SECTOR);
    writeSector(map, MAP_SECTOR);
}

// F03
void findDir(char * parent, char * current, char * filename, int * idx, int * result) 
{
    char name[MAX_FILENAME + 1];
    char i = 0;
    char end;
    char dir[SECTOR_SIZE];
    char j;
    char found;
    if (filename[*idx] == '/'){*idx++;}
    while ((filename[*idx + i] != '\0') && (filename[*idx + i] != '/')) {
        name[i] = filename[*idx + i];
        i++;
    }
    // end null terminator.
    end = filename[*idx + i] == '\0';
    name[i] = '\0';
    j = i;
    readSector(dir, DIRS_SECTOR);
    // Find file
    found = FALSE;
    i = 0;
    while ((!found) && (i < MAX_DIRS)) {
        char buff[4];
        int k = 0;
        if ((dir[i * ENTRY_LENGTH] == *parent) && (compare(name, dir + (i * ENTRY_LENGTH) + 1, MAX_FILENAME))){
            found = TRUE;
        } else {i++;}
    }
    if (found) {
        *current = i;
        if (end) {*result = SUCCESS;} 
        else {// Recursively search 
            *parent = *current;
            *idx = *idx + j + 1;
            findDir(parent, current, filename, idx, result);
        }
    } else {
        *result = NOT_FOUND;
        *current = *parent;
    }
}

void makeDirectory(char *path, int *result, char parentIndex) 
{
    char parent = parentIndex;
    int i = 0;
    char current;
    findDir(&parent, &current, path, &i, result);
    if (*result == SUCCESS) {
        *result = ALREADY_EXISTS;
    } else {
        char filename[MAX_FILENAME + 1];
        int j = 0;
        while ((path[i + j] != '\0') && (path[i + j] != '/')) {
            filename[j] = path[i + j];
            j++;
        }
        filename[j] = '\0';
        if (path[i + j] == '/') {*result = NOT_FOUND;} 
        else {
            char dir[SECTOR_SIZE];
            current = 0;
            readSector(dir, DIRS_SECTOR);
            while ((dir[current * ENTRY_LENGTH + 1] != '\0') && (current < MAX_DIRS)){current++;}
            if (current < MAX_DIRS) {
                dir[current * ENTRY_LENGTH] = parent;
                j = 0;
                while (filename[j] != '\0') {
                    dir[current * ENTRY_LENGTH + 1 + j] = filename[j];
                    j++;
                }
                writeSector(dir, DIRS_SECTOR);
                *result = SUCCESS;
            } else {*result = INSUFFICIENT_ENTRIES;}
        }
    }
}

void deleteDirectory(char *path, int *success, char parentIndex) 
{
    char parent = parentIndex;
    int i = 0;
    char current;
    findDir(&parent, &current, path, &i, success);
    if (*success == SUCCESS) {deleteDirectoryIndex(current);}
}

void deleteDirectoryIndex(char current) 
{
    char dirs[SECTOR_SIZE];
    char files[SECTOR_SIZE];
    int i;
    
    readSector(dirs, DIRS_SECTOR);
    // null terminator.
    dirs[current * ENTRY_LENGTH] = 0x00;
    dirs[current * ENTRY_LENGTH + 1] = '\0';
    writeSector(dirs, DIRS_SECTOR);
    readSector(files, FILES_SECTOR);
    for (i = 0; i < MAX_FILES; i++) {
        if ((files[i * ENTRY_LENGTH] == current) && (files[i * ENTRY_LENGTH + 1] != '\0')){
            deleteFileIndex(i);
        }
    }
    for (i = 0; i < MAX_DIRS; i++) {
        if ((dirs[i * ENTRY_LENGTH] == current) && (dirs[i * ENTRY_LENGTH + 1] != '\0')){
            deleteDirectoryIndex(i);
        }
    }
}

// F04
void executeProgram (char *path, int *result, char parentIndex, char async) 
{ 
    struct PCB* pcb;
    int segment;
    int i, fileIndex;
    char buffer[MAX_SECTORS * SECTOR_SIZE];
    readFile(buffer, path, result, parentIndex);
    
    if (*result != NOT_FOUND) {
        setKernelDataSegment();
        segment = getFreeMemorySegment();
        restoreDataSegment();
        
        fileIndex = *result; 
        if (segment != NO_FREE_SEGMENTS) {
            setKernelDataSegment();
            pcb = getFreePCB();
            pcb->index = fileIndex;
            pcb->state = STARTING;
            pcb->segment = segment;
            pcb->stackPointer = 0xFF00;
            if (async) {
                pcb->parentSegment = NO_PARENT;
            } else {
                pcb->parentSegment = running->segment;
            }
            addToReady(pcb);
            restoreDataSegment();
            for (i = 0; i < SECTOR_SIZE * MAX_SECTORS; i++) {
                putInMemory(segment, i, buffer[i]);
            }
            initializeProgram(segment);
            *result = segment;
            if (!async)
                sleep();
        }
        else {
            *result = INSUFFICIENT_SEGMENTS;
        }
    }
}

void terminateProgram (int *result) 
{
    int parentSegment;
    
    setKernelDataSegment();
    parentSegment = running->parentSegment;
    releaseMemorySegment(running->segment);
    releasePCB(running);
    restoreDataSegment();
    if (parentSegment != NO_PARENT) {
        resumeProcess(parentSegment, result);
    }
    yieldControl();
}

// F05
void putArgs (char curdir, char argc, char **argv) 
{
    char args[SECTOR_SIZE];
    int i, j, p;
    clear(args, SECTOR_SIZE);
    args[0] = curdir;
    args[1] = argc;
    i = 0; j = 0;
    for (p = 2; p < ARGS_SECTOR && i < argc; ++p) {
        args[p] = argv[i][j];
        if (argv[i][j] == '\0') {
            ++i; j = 0;
        } else {++j;}
    }
    writeSector(args, ARGS_SECTOR);
}

void getCurdir (char *curdir) 
{
    char args[SECTOR_SIZE];
    readSector(args, ARGS_SECTOR);
    *curdir = args[0];
}

void getArgc (char *argc) 
{
    char args[SECTOR_SIZE];
    readSector(args, ARGS_SECTOR);
    *argc = args[1];
}

void getArgv (char index, char *argv) 
{
    char args[SECTOR_SIZE];
    int i, j, p;
    readSector(args, ARGS_SECTOR);
    i = 0; j = 0;
    for (p = 2; p < ARGS_SECTOR; ++p) {
        if (i == index) {
            argv[j] = args[p];
            ++j;
        }
        if (args[p] == '\0') {
            if (i == index) {break;} 
            else {++i;}
        }
    }
}

// F06
void yieldControl () 
{
    interrupt(0x08, 0, 0, 0, 0);
}

void sleep () 
{
    setKernelDataSegment();
    running->state = PAUSED;
    restoreDataSegment();  
    yieldControl();
}

void pauseProcess (int segment, int *result) 
{
    struct PCB *pcb;
    int res;
    
    setKernelDataSegment();
    pcb = getPCBOfSegment(segment);
    if (pcb != NULL && pcb->state != PAUSED) {
        pcb->state = PAUSED;
        res = SUCCESS;
    }
    else {
        res = NOT_FOUND;
    }
    restoreDataSegment();
    
    *result = res;
}

void handleTimerInterrupt(int segment, int stackPointer) 
{
    struct PCB *currPCB;
    struct PCB *nextPCB;
    
    setKernelDataSegment();
    currPCB = getPCBOfSegment(segment);
    currPCB->stackPointer = stackPointer;
    if (currPCB->state != PAUSED) {
        currPCB->state = READY;
        addToReady(currPCB);
    }
    
    do {
        nextPCB = removeFromReady();
    }  
    while (nextPCB != NULL && (nextPCB->state == DEFUNCT || nextPCB->state == PAUSED));

    if (nextPCB != NULL) {
        nextPCB->state = RUNNING;
        segment = nextPCB->segment;
        stackPointer = nextPCB->stackPointer;
        running = nextPCB;
    }
    else {
        running = &idleProc;
    }
    restoreDataSegment();
    
    returnFromTimer(segment, stackPointer);
}

void resumeProcess (int segment, int *result) 
{
    struct PCB *pcb;
    int res;
    
    setKernelDataSegment();
    pcb = getPCBOfSegment(segment);
    if (pcb != NULL && pcb->state == PAUSED) {
        pcb->state = READY;
        addToReady(pcb);
        res = SUCCESS;
    }
    else {
        res = NOT_FOUND;
    }
    restoreDataSegment();
    
    *result = res;
}

void killProcess (int segment, int *result) 
{
    struct PCB *pcb;
    int res;
    
    setKernelDataSegment();
    pcb = getPCBOfSegment(segment);
    if (pcb != NULL) {
        releaseMemorySegment(pcb->segment);
        releasePCB(pcb);
        res = SUCCESS;
    }
    else {
        res = NOT_FOUND;
    }
    restoreDataSegment();
    
    *result = res;
}

// F07
void writeln(char *string){
	printString(string);
	printString("\n\r");
}

char compare(char * arr1, char * arr2, int length) 
{
    int i = 0;
    char equal = TRUE;
    while ((equal) && (i < length)) {
        equal = arr1[i] == arr2[i];
        if (equal) {
            if (arr1[i] == '\0') {
                i = length;
            }
        }
        i++;
    }
    return equal;
}
