#define SECTOR_SIZE 512
#define MAX_SECTORS 16
#define SUCCESS 0
#define TRUE 1
#define FALSE 0

char strcmp(char * arr1, char * arr2, int len);
int div(int a, int b);

void main() {
    char curdir;
    char argc;
    char argv[4][32];
    char outBuff[SECTOR_SIZE * MAX_SECTORS];
    char inBuff[128];
    char writeMode = 0;
    int succ;
    int i, j, k;
    int sectors;
    int succ = 0;
    
    enableInterrupts();
    //getCurdir
    interrupt(0x21, 0x21, &curdir, 0, 0);
    //getArgc
    interrupt(0x21, 0x22, &argc, 0, 0);
    for (i = 0; i < argc; i++) {
		//getArgv
        interrupt(0x21, 0x23, i, argv[i], 0);
    }
    if (argc > 0) {
        if (argc > 1) {
			// Bandingkan argumen ke-2 dengan -w
            if (strcmp("-w", argv[1], 2)){
				writeMode = 1;
			} else{
				interrupt(0x21, 0x0, "Flag tidak valid!\n", 0, 0);
				interrupt(0x21, 0x0, "Flag yang valid : -w\n", 0, 0);
			}
        }
        
        if (writeMode == 0) { //Membaca file
			//readFile
            interrupt(0x21, (curdir << 8) | 0x04, outBuff, argv[0], &succ);
            if (succ == SUCCESS) {
				//printString
				interrupt(0x21, 0x0, "Isi file Anda :\n", 0, 0);  
                interrupt(0x21, 0x0, outBuff, 0, 0);      
                interrupt(0x21, 0x0, "\n", 0, 0);      
            } else {
				interrupt(0x21, 0x0, "File tidak ditemukan!\n", 0, 0);
			}
        } else if (writeMode == 1) { //Menulis file
            for (i = 0; i < SECTOR_SIZE * MAX_SECTORS; i++) {
				// Kosongkan outBuff terlebih dahulu
				outBuff[i] = '\0';
			}
			//printString
            interrupt(0x21, 0x0, "\n", 0, 0);
            k = 0;
            //readString
            interrupt(0x21, 0x01, inBuff, 0, 0);
            while (inBuff[0] != '\0') {
                //readString sampai inBuff bernilai null
                j = 0;
                while (inBuff[j] != '\0') {
                    outBuff[k] = inBuff[j];
                    j++; k++;
                }
                outBuff[k] = '\n';
                k++; 
                interrupt(0x21, 0x01, inBuff, 0, 0);
            }
            outBuff[k - 1] = '\0';
            //printString
            interrupt(0x21, 0x0, "File Anda tersimpan!\n", 0, 0);
            //for Debug
            interrupt(0x21, 0x0, "Isi file Anda : ", 0, 0);
            interrupt(0x21, 0x0, outBuff, 0, 0);
            sectors = div(k, SECTOR_SIZE) + 1;
            //printString writeFile
            interrupt(0x21, (curdir << 8) | 0x05, outBuff, argv[0], &sectors);
        }
    } else{
		//printString
		interrupt(0x21, 0x0, "Cara penggunaan (readmode): 'cat <file>'\n", 0, 0);
		interrupt(0x21, 0x0, "Cara penggunaan (writemode): 'cat <file> -w'\n", 0, 0);
	}
    //terminateProgram(0) BX = 0 AL = 0x07;
    interrupt(0x21, (0x00 << 8) | 0x07, &succ, 0, 0);
}

int div(int a, int b) {
	// mengembalikan a div b
    int q = 0;
    while(q*b <=a) {
		q = q+1;
	}
    return q-1;
}

char strcmp(char * s1, char * s2, int len) {
    int i = 0;
    int equal = TRUE;
    while ((equal == TRUE) && (i < len)) {
		// Bandingkan kedua string
        equal = (s1[i] == s2[i]);
        if (equal) {
            if (s1[i] == '\0') {
				i = len;
			}
        }
        i++;
    }
    return equal;
}
