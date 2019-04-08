#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 32
#define MAX_FILENAME 15
#define MAX_SECTORS 16
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 0x100
#define DIR_SECTOR 0x101
#define FILE_SECTOR 0x102
#define SECTOR_SECTOR 0x103
#define SECTORS_SECTOR 0x103
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define ALREADY_EXISTS -2
#define ALREADY_EXIST -2
#define INSUFFICIENT_ENTRIES -3
#define EMPTY 0x00
#define USED 0xFF
#define SUCCESS 0
#define NULL ((void*)0)
#define ARGS_SECTOR 512

/* PROTOTYPE FUNGSI */

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void clear(char *buffer, int length);
void executeProgram(char *path, int segment, int *result, char parentIndex);

// Fungsi untuk Milestone 2

void findDirectory(char *path, char *result, char parentIndex, int *attempt);
void findFile(char *id, char *result, char parentIndex, int *attempt);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex); 
void executeProgram(char *path, int segment, int *result, char parentIndex);
void terminateProgram (int *result);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteFileByIndex(char index, int *attempt);
void deleteFile(char *path, int *result, char parentIndex);
void deleteDirectoryByIdx(char index, int *attempt);
void deleteDirectory(char *path, int *success, char parentIndex);

//Prototype fungsi 4 syscall
void putArgs(char curdir, char argc, char **argv);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char index, char *argv);

//Fungsi Pembantu
int strlen(char *string);
void writeln(char *string);

int main() {
	//printLogo();
	
	int succ;
	makeInterrupt21();
	
	putArgs(0xFF, 0, 0);
	executeProgram("shell",0x2000, &succ, 0xFF);
	
	while (1);
}

void printLogo(){
	writeln("");
	printString("******************************************\n\r");
	printString("*                  ****                  *\n\r");
	printString("*                 *    *   ****          *\n\r");
	printString("*                 *    *  *              *\n\r");
	printString("* *           *    ****    ***           *\n\r");
	printString("*  *         *                *          *\n\r");
	printString("*   *       *             ****           *\n\r");
	printString("*    *     *                             *\n\r");
	printString("*     *   *  *   *   *   *   *   *       *\n\r");
	printString("*      * *  * *  **  *  * *  ** **       *\n\r");
	printString("*       *  ***** * * * ***** * * *       *\n\r");
	printString("*          *   * *  ** *   * *   *       *\n\r");
	printString("*          *   * *   * *   * *   *       *\n\r");
	printString("*                                        *\n\r");
	printString("******************************************\n\r");
	writeln("");
	printString("*******PRESS ANY KEY TO CONTINUE...*******\n\r");
	writeln("");
	writeln("");
	​interrupt(0x16,0, 0, 0, 0);
}

//New handleInterrupt21
void handleInterrupt21 (int AX, int BX, int CX, int DX) {
   char AL, AH;
   AL = (char) (AX);
   AH = (char) (AX >> 8);

   switch (AL) {
      case 0x00:
         printString(BX);
         break;
      case 0x01:
         readString(BX);
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
         executeProgram(BX, CX, DX, AH);
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
         putArgs(BX, CX);
         break;
      case 0x21:
         getCurdir(BX);
         break;
      case 0x22:
         getArgc(BX);
         break;
      case 0X23:
         getArgv(BX, CX);
         break;
      default:
         printString("Invalid interrupt");
   }
} 

//implementasi printString dan readString
void printString(char *string){
	char ch;
	int i;
	
	i = 0;
	while(string[i] != '\0'){
		ch = string[i];
		//Tampilkan ke layar per karakter sampai null
		interrupt(0x10, 0xE00+ch, 0, 0, 0);
		i++;
	}
}

void readString(char *string){
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
	printString(string);
	printString("\n\r");
}

//mod and div
int mod(int a, int b) {
	if(a >= 0){
		while(a >= b) {
			a = a - b;
		}
	} else{
		while(a < 0){
			a = a + b;
		}
	}
	return a;
}

int div(int a, int b) {
	int c = 0;
	while(c*b <= a) {
		c = c+1;
	}
	return c-1;
}

//Diberikan pada Spek
void readSector(char *buffer, int sector){
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector){
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

//New readFile
void readFile(char *buffer, char *path, int *result, char parentIndex){
	char currentName[MAX_FILENAME];
    int equal = TRUE;
    int i = 0;
    int found = FALSE;
    char res;
    int attempt;
    char sector[SECTORS_SECTOR];

    // Path tidak diisi apapun
    if (path[0] == 0){
        *result = NOT_FOUND;
    }

    i = 0;
    // Copy path terkiri atau nama filenya
    while (path[i] != '\0' && path[i] != '/')
    {
        currentName[i] = path[i];
    }

    currentName[i] = '\0';
    if (path[i] == '/')
    { 
		//Cari file secara rekursif
        findDirectory(currentName, &result, parentIndex, &attempt);
        if (attempt == NOT_FOUND)
        {
            *result == NOT_FOUND;
            return;
        }
        readFile(buffer, path + i + 1, result, res);
    }
    else
    { 
		//Menemukan lokasi file
        readSector(sector, SECTORS_SECTOR);
        findFile(currentName, &res, parentIndex, &attempt);
        if (attempt == NOT_FOUND)
        {
            *result == NOT_FOUND;
            return;
        }
        
        //Memindahkan file ke buffer
        i = 0;
        while ((i < 0x10) && (sector[res * DIR_ENTRY_LENGTH + i] != 0)){
            readSector(buffer + i * SECTOR_SIZE, sector[res * DIR_ENTRY_LENGTH + i]);
            i++;
        }
        
        *result = SUCCESS;
    }
}

//New writeFile
void writeFile(char *buffer, char *path, int *sectors, char parentIndex){
	char sector[SECTOR_SIZE];
    char file[SECTOR_SIZE];
    char map[SECTOR_SIZE];
    char name[MAX_FILENAME];
    char index_file, res, index_sector;
    int i, j, count, buff, temp;
    int found;
    
    count = 0;
    readSector(map, MAP_SECTOR);
    i = 0;
    // Cari apakah tersedia entry pada map
    do{
		if (map[i] == USED)
            continue;
        count++;
	}while((i < SECTOR_SIZE) && (count < DIR_ENTRY_LENGTH));
    
    if (count < DIR_ENTRY_LENGTH){
        *sectors = INSUFFICIENT_SECTORS;
        return;
    }

    // Cari entry kosong pada sector
    found = FALSE;
    readSector(file, FILE_SECTOR);
    for (i = 0; i < MAX_FILES; i++){
        if (file[i * DIR_ENTRY_LENGTH + 1] == 0){
            found = TRUE;
            index_file = i;
            break;
        }
    }
    if (found == FALSE){
        *sectors = INSUFFICIENT_ENTRIES;
        return;
    }

    // Path tidak diisi apapun
    if (strlen(path)){
        *sectors = NOT_FOUND;
        return;
    }

    // Copy direktori pertama pada path
    // /abc/def maka copy abc
    i = 0;
    while ((path[i] != '\0') && (path[i] != '/')){
        name[i] = path[i];
        i++;
    }
    name[i] = '\0';

    if (path[i] == '/'){
		//Secara rekursif write file
        findDirectory(name, &res, parentIndex, &temp);
        if (temp == NOT_FOUND){
            *sectors = NOT_FOUND;
            return;
        }
        writeFile(buffer, path + i + 1, sectors, res);

    }else{
		//Nama file tercopy
        findFile(name, &res, parentIndex, &temp);
        if (temp == 0){
            *sectors = ALREADY_EXIST;
            return;
        }


        file[index_file * DIR_ENTRY_LENGTH] = parentIndex;
        for (i = 0; i < MAX_FILENAME; i++)
            file[index_file * DIR_ENTRY_LENGTH + 1 + i] = path[i];
        readSector(sector, SECTOR_SECTOR);
        buff = strlen(buffer) / SECTOR_SIZE + 1;
        
        
        for (i = 0; i < buff; i++){
            for (j = 0; j < MAX_SECTORS; j++)
                if (map[j] == EMPTY)
                    break;
            writeSector(buffer + i * SECTOR_SIZE, j);
            sector[index_file * DIR_ENTRY_LENGTH + i] = j;
            map[j] == USED;
            j++;
        }
        
        //writeSector
        writeSector(sector, SECTOR_SECTOR);
        writeSector(file, FILE_SECTOR);
        writeSector(map, MAP_SECTOR);
        *sectors = 1;
    }
}

void findDirectory(char *path, char *result, char parentIndex, int *attempt) {
    char dir[SECTOR_SIZE];
    char name[MAX_FILENAME];
    int equal = TRUE;
    int i,j;
		
	// Membaca sektor
    readSector(dir, DIR_SECTOR);
    
    // Path tidak diisi apapun
    if (strlen(path) == 0){
        *attempt = NOT_FOUND;
        return;
    }

    // Cek direktori pertama terbaca
    i = 0;
    while ((path[i] != 0) && (path[i] != '/')){
        name[i] = path[i];
        i++;
    }
    name[i] = 0;

    if (path[i] == '/') {
		// Cari directory secara rekursif
        findDirectory(name, result, parentIndex, attempt);
        if (attempt == NOT_FOUND)
            return;
        parentIndex = *result;
        findDirectory(path + i + 1, result, parentIndex, attempt);
    } else{
        // Cek dengan parentIndex
        for (i = 0; i < MAX_FILES; i++){
            if (dir[i * DIR_ENTRY_LENGTH] != parentIndex)
                continue;

            equal = TRUE;
            for (j = 0; j < MAX_FILENAME; j++){
                if ((equal) && ((path[j] != 0) || (dir[i * DIR_ENTRY_LENGTH + 1 + j]))){
                    equal = equal && (path[j] == dir[i * DIR_ENTRY_LENGTH + 1 + j]);
                } else{
                    break;
                }
            }

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
    int i,j;
    int equal;

    readSector(files, FILE_SECTOR);

    // Path tidak diisi apapun
    if (strlen(path) == 0){
        *attempt = NOT_FOUND;
        return;
    }

    // Cek seluruh file yang ada di parentIndex
    for (i = 0; i < MAX_FILES; i++){
        if (files[i * DIR_ENTRY_LENGTH + 1] == 0)
            continue;

        if (files[i * DIR_ENTRY_LENGTH] != parentIndex)
            continue;

        equal = TRUE;
        j = 0;
        while((j < MAX_FILENAME) && (equal) && ((path[j] != 0) || (files[i * DIR_ENTRY_LENGTH + 1 + j] != 0))){
			equal = equal && (path[j] == files[i * DIR_ENTRY_LENGTH + 1 + j]);
			j++;
		}
        
        // File ketemu
        if (equal == TRUE){
            *result = i;
            *attempt = 0;
            return;
        }
    }
    *attempt = NOT_FOUND;
}

void clear(char *buffer, int length) {
	int i;
	for(i = 0; i < length; ++i) {
		buffer[i] = EMPTY;
	}
}

//string length
int strlen(char *string){
	int i = 0;
	while(string[i] != '\0'){
		i++;
	}
	return i;
}

//New executeProgram
void executeProgram(char *path, int segment, int *result, char parentIndex){
    int i;
    char buffer[DIR_ENTRY_LENGTH * SECTOR_SIZE];
    clear(buffer, DIR_ENTRY_LENGTH * SECTOR_SIZE);

	//readFile
    readFile(buffer, path, result, parentIndex);
    if (*result == 0){
        for (i = 0; i < sizeof(buffer); ++i){
			putInMemory(segment, i, buffer[i]);
		}
        launchProgram(segment);
    }
}


//New function : terminateProgram
void terminateProgram (int *result) {
   char shell[6];
   shell[0] = 's';
   shell[1] = 'h';
   shell[2] = 'e';
   shell[3] = 'l';
   shell[4] = 'l';
   shell[5] = '\0';
   executeProgram(shell, 0x2000, result, 0xFF);
}

void makeDirectory(char *path, int *result, char parentIndex){
	char dir[SECTOR_SIZE];
    char dirName[MAX_FILENAME];
    char equal, res;
    int idx, temp, i;
    int found = FALSE;

	// Path tidak berisi apa pun
    if (strlen(path) == 0){
        *result = NOT_FOUND;
        return;
    }

    // Cari entry kosong pada dir
    readSector(dir, DIR_SECTOR);
    for (i = 0; i < MAX_FILES; i++){
        if (dir[i * DIR_ENTRY_LENGTH + 1] == 0){
            found = TRUE; 
            idx = i;
            break;
        }
    }
    
    if (found == FALSE){
		//Entry tidak cukup
        *result = INSUFFICIENT_ENTRIES;
        return;
    }

    // Copy directory pertama
    // /abc/def copy abc
    i = 0;
    while ((path[i] != '\0') && (path[i] != '/')){
        dirName[i] = path[i];
        i++;
    }
    dirName[i] = '\0';

    // cari index directory parentIndex
    findDirectory(dirName, &res, parentIndex, &temp);
    if (path[i] == '/'){
        if (temp == NOT_FOUND){
            *result = NOT_FOUND;
            return;
        }
        makeDirectory(path + i + 1, result, res);
    }else{
        if (temp != NOT_FOUND){
			//Directory parent tidak ada
            *result = NOT_FOUND;
            return;
        } else if(temp == ALREADY_EXISTS){
			//Directory sudah ada
			*result = ALREADY_EXISTS;
            return;
		}
        
        dir[idx * DIR_ENTRY_LENGTH] = parentIndex;
        for (i = 0; i < MAX_FILENAME; i++){
            if (path[i] != 0){
                dir[idx * DIR_ENTRY_LENGTH + 1 + i] = path[i];
            } else{
                break;
            }
        }
        
        //writeSector
        writeSector(dir, DIR_SECTOR);
        *result = 0;
    }
}

void deleteFile(char *path, int *result, char parentIndex){
	char fileName[MAX_FILENAME];
    char idx;
    int i;
	int tempResult;

    char terminator;
    
    // Path tidak diisi apa pun
    if (path[0] == '\0'){
        *result = -1;
        return;
    }

    // Ambil nama file saja
    // terminator = '/';
	i = 0;
	while ((path[i] != '/') && (path[i] != '\0')){
		fileName[i] = path[i];
		i++;
	}
	terminator = path[i];
    fileName[i] = '\0';

    if (path[i] == '/'){
		//Proses direktori anak secara rekursif
        findDirectory(fileName, &idx, parentIndex, &tempResult);
        if (tempResult == -1){
            *result = -1;
            return;
        }
        deleteFile(path+i+1, result, idx);

    }else{
		//Cari file
        findFile(fileName, &idx, parentIndex, &tempResult);
        if (tempResult == -1){
            *result = -1;
            return;
        }
        deleteFileByIndex(idx, result);
    }
}

void deleteFileByIndex(char index, int *result){
    int i, j;
    char file[SECTOR_SIZE];
    char map[SECTOR_SIZE];
    char sector[SECTOR_SIZE];

	//readSector
    readSector(file, FILE_SECTOR);
    if (file[index * DIR_ENTRY_LENGTH + 1] == 0){
        *result = 0;
        return;
    }
    readSector(map, MAP_SECTOR);
    readSector(sector, SECTOR_SECTOR);
    
    file[index * DIR_ENTRY_LENGTH + 1] = '\0';

    // Proses sektor map
    i = 0;
    while(i < DIR_ENTRY_LENGTH){
		if (sector[index * DIR_ENTRY_LENGTH + i] == 0)
            break;
        map[sector[index * DIR_ENTRY_LENGTH + i]] = EMPTY;
        i++;
	}
    
    //writeSector
    writeSector(file, FILE_SECTOR);
    writeSector(map, MAP_SECTOR);
    *result = 0;
}

void deleteDirectory(char *path, int *success, char parentIndex){
    char res;
    char dirName[MAX_FILENAME];
    int i;
    char terminator;
    int tempSuccess;
    
    // Path tidak diisi apa pun
    if (path[0] == '\0'){
        *success = NOT_FOUND;
        return;
    }

    // Copy directory pertama yang terbaca ke dirName
    // Misal: copy 'abc' dari /abc/def
    i = 0;
    while ((path[i] != 0) && (path[i] != '/')){
        dirName[i] = path[i];
        i++;
    }
    dirName[i] = 0;

    // Cari directory secara rekursif
    if (path[i] == '/'){
        findDirectory(dirName, &res, parentIndex, &tempSuccess);
        if (tempSuccess == NOT_FOUND){
            *success = NOT_FOUND;
            return;
        }
        deleteDirectory(path + i + 1, success, res);
    }else{
		//Nama direktori terakhir yang akan didelete tercopy
		//Delete directory by index
        findDirectory(dirName, &res, parentIndex, &tempSuccess);
        if (tempSuccess == NOT_FOUND){
            *success = NOT_FOUND;
            return;
        }
        deleteDirectoryByIdx(res, success);
    }
}

void deleteDirectoryByIdx(char index, int *result){
    char i, j;
    char sector[SECTOR_SIZE];
    char dir[SECTOR_SIZE];
    char file[SECTOR_SIZE];
    char map[SECTOR_SIZE];
    
    //readSector
    readSector(dir, DIR_SECTOR);
    readSector(map, MAP_SECTOR);
    readSector(sector, SECTOR_SECTOR);
    readSector(file, FILE_SECTOR);
    
    if (dir[index * DIR_ENTRY_LENGTH + 1] == 0){
        *result = 0;
        return;
    }
    
    dir[index * DIR_ENTRY_LENGTH + 1] = '\0';
    
    // Delete file-file yang ada pada directory
    i = 0;
    while(i < MAX_FILES){
		if (file[i * DIR_ENTRY_LENGTH] == index){
			deleteFileByIndex(i, result);
		}
		i++;
	}
        
    // Menghapus directory anak secara rekursif 
    i = 0;
    while(i < MAX_FILES){
		if (dir[i * DIR_ENTRY_LENGTH] == index)
            deleteDirectoryByIdx(i, result);
        i++;
	}
        
	//writeSector
    writeSector(file, FILE_SECTOR);
    writeSector(map, MAP_SECTOR);
    writeSector(dir, DIR_SECTOR);
    *result = 0;
}

//Implementasi 4 Syscall
void putArgs (char curdir, char argc, char **argv) {
   char args[SECTOR_SIZE];
   int i, j, p;
   clear(args, SECTOR_SIZE);

   args[0] = curdir;
   args[1] = argc;
   i = 0;
   j = 0;
   for (p = 1; p < ARGS_SECTOR && i < argc; ++p) {
      args[p] = argv[i][j];
      if (argv[i][j] == '\0') {
         ++i;
         j = 0;
      }
      else {
         ++j;
      }
   }

   writeSector(args, ARGS_SECTOR);
}

void getCurdir (char *curdir) {
   char args[SECTOR_SIZE];
   readSector(args, ARGS_SECTOR);
   *curdir = args[0];
}

void getArgc (char *argc) {
   char args[SECTOR_SIZE];
   readSector(args, ARGS_SECTOR);
   *argc = args[1];
}

void getArgv (char index, char *argv) {
   char args[SECTOR_SIZE];
   int i, j, p;
   readSector(args, ARGS_SECTOR);

   i = 0;
   j = 0;
   for (p = 1; p < ARGS_SECTOR; ++p) {
      if (i == index) {
         argv[j] = args[p];
         ++j;
      }

      if (args[p] == '\0') {
         if (i == index) {
            break;
         }
         else {
         ++i;
         }
      }
   }
} 

