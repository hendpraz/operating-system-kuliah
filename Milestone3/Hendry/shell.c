#define MAX_ARGS 8
#define MAX_ARG_LENGTH 32
#define BUFF_SIZE 64
#define SUCCESS 0
#define TRUE 1

char strcmp(char *str1, char * str2, int length);
int strlen(char *string);

int main() {
	int off;
    int i = 0;
    int segments[8];
    char dest;
    char succ = 0;
    char curdir;
    char argc = 0;
    char buffer[BUFF_SIZE];
    char command[BUFF_SIZE];
    char arguments[36];
    char * argv[MAX_ARGS];
    //Segment memori di mana proses data berada
    segments[0] = 0x2000; 
    segments[1] = 0x3000;
    segments[2] = 0x4000; 
    segments[3] = 0x5000;
    segments[4] = 0x6000; 
    segments[5] = 0x7000;
    segments[6] = 0x8000; 
    segments[7] = 0x9000;
    
    enableInterrupts();
    //getCurdir
    interrupt(0x21, 0x21, &curdir, 0, 0);
    while (1) {
        for (i = 0; i < BUFF_SIZE; i++) {
			//Inisialisasi nilai pada buffer
			buffer[i] = '\0';
		}
		//printString dollar ($) selama shell berjalan
        interrupt(0x21, 0x0, "$ ", 0, 0);
        //readString selama shell berajalan
        interrupt(0x21, 0x1, buffer, 1, 0);
        i = 0;
        while ((buffer[i] != ' ') && (buffer[i] != '\0')) {
			command[i] = buffer[i]; i++;
		}
        command[i] = '\0';
        if (i > 0) {
            if ((strcmp(command, "cd", 2)) && (buffer[i] != '\0')) {
				// CHANGE DIRECTORY
                i++; 
                off = i;
                while ((buffer[i] != ' ') && (buffer[i] != '\0')) {
					arguments[i - off] = buffer[i]; 
					i++;
				}
                arguments[i - off] = '\0';

                if ((i - off == 0)) {
					//putArgs
                    interrupt(0x21, 0x20, 0xFF, argc, argv);
                    interrupt(0x21, 0x0, "Sekarang Anda berada di root dir\n", 0, 0);
                    curdir = 0xFF;
                } else {
                    dest = -1;
                    interrupt(0x21, (curdir << 8) | 0x90, arguments, &succ, &dest);
                    if (succ) {
                        interrupt(0x21, 0x20, dest, argc, argv);
                        curdir = dest;
                    } else {
                        interrupt(0x21, 0x0, "Directory NOT Found ", 0, 0);
                        interrupt(0x21, 0x0, arguments, 0, 0);
                        interrupt(0x21, 0x0, "\n", 0, 0);
                    }
                }
            } else if (strcmp(command, "pause", 5)) {
                i++;
                off = i;
                while ((buffer[i] != ' ') && (buffer[i] != '\0')) {
                    arguments[i - off] = buffer[i];
                    i++;
                }
                arguments[i - off] = '\0';
                if (strlen(arguments) == 1) {
                    int index = arguments[0] - '0';
                    if ((index >= 0) && (index <= 7)) {
                        int segment = segments[index];
                        interrupt(0x21, 0x32, segment, &succ, 0);
                        if (succ == SUCCESS) {
							//proses ketemu pada segment, printString
							interrupt(0x21, 0x0, "Process telah di-pause", 0, 0);
						} else {
							//proses tidak ditemukan di segment, printString
							interrupt(0x21, 0x0, "Process tidak ditemukan!\n", 0, 0);
						}
                    } else {
						interrupt(0x21, 0x0, "PID Process tidak valid!\n", 0, 0);
					}
                } else {
					interrupt(0x21, 0x0, "PID Process tidak valid!\n", 0, 0);
				}
            } else if (strcmp(command, "resume", 6)) {
                i++; off = i;
                while ((buffer[i] != ' ') && (buffer[i] != '\0')) {
                    arguments[i - off] = buffer[i]; i++;
                }
                arguments[i - off] = '\0';
                if (strlen(arguments) == 1) {
                    int index = arguments[0] - '0';
                    if ((index >= 0) && (index <= 7)) {
                        int segment = segments[index];
                        //resume
                        interrupt(0x21, 0x33, segment, &succ, 0);
                        if (succ == SUCCESS) {
							interrupt(0x21, 0x0, "Process di-resume\n", 0, 0);
							//sleep
							interrupt(0x21, 0x31, 0, 0, 0);
						} 
                        else {
							interrupt(0x21, 0x0, "Process tidak ditemukan!\n", 0, 0);
						}
                    } else {
						interrupt(0x21, 0x0, "PID tidak valid!\n", 0, 0);
					}
                } else {
					interrupt(0x21, 0x0, "PID tidak valid!\n", 0, 0);
				}
            } else if (strcmp(command, "kill", 4)) {
                i++; off = i;
                while ((buffer[i] != ' ') && (buffer[i] != '\0')) {
                    arguments[i - off] = buffer[i]; i++;
                }
                arguments[i - off] = '\0';
                if (strlen(arguments) == 1) {
                    int index = arguments[0] - '0';
                    if ((index >= 0) && (index <= 7)) {
                        int segment = segments[index];
                        interrupt(0x21, 0x34, segment, &succ, 0);
                        if (succ == SUCCESS){
							interrupt(0x21, 0x0, "Process di-kill\n", 0, 0);
						} 
                        else{
							interrupt(0x21, 0x0, "Process tidak ditemukan!\n", 0, 0);
						}
                    } else {
						interrupt(0x21, 0x0, "PID tidak valid!\n", 0, 0);
					}
                } else {
					interrupt(0x21, 0x0, "PID tidak valid!\n", 0, 0);
				}
            } else {
				//Selain pause, resume, kill
				//Dapat memanggil program lain yang tersedia
                char buff[MAX_ARGS][MAX_ARG_LENGTH];
                char async = 0;
                argc = 0;
                while (buffer[i] != '\0') {
                    i++; off = i;
                    while ((buffer[i] != ' ') && (buffer[i] != '\0')) {
                        buff[argc][i - off] = buffer[i]; 
                        i++;
                    }
                    buff[argc][i - off] = '\0';
                    if (strcmp(buff[argc], "&", 1)){
						async = 1;
					} else{
                        argv[argc] = buff[argc]; argc++;
                        if (argc >= MAX_ARGS){break;}
                    }
                }
                //putArgs
                interrupt(0x21, 0x20, curdir, argc, argv);

				//Cari file yang sesuai command
                interrupt(0x21, 0xFF91, command, &succ, &dest);
                if (succ) {
                    if (async) {
						//excecute program, async = 0
						interrupt(0x21, 0xFF92, command, &succ, 0);
					} 
                    else {
						//Eksecute program
						interrupt(0x21, 0xFF06, command, &succ, 0);
					}
                } else {
					//printString
                    interrupt(0x21, 0x0, "Command Anda salah : ", 0, 0);
                    interrupt(0x21, 0x0, command, 0, 0);
                    interrupt(0x21, 0x0, "\n", 0, 0);
                    interrupt(0x21, 0x0, "Command tersedia : cat, echo, ls, mkdir, rm, pause, resume, kill\n", 0, 0);
                }
            }
        }
    }
    //terminate
    interrupt(0x21, (0x00 << 8) | 0x07, &succ, 0, 0);
}

char strcmp(char * str1, char * str2, int length) {
    char equal = TRUE;
    int i = 0;
    while ((equal) && (i < length)) {
        equal = str1[i] == str2[i];
        if (equal) {
            if (str1[i] == '\0') {i = length;}
        }
        i++;
    }
    return equal;
}

int strlen(char *string) {
    int len = 0;
    while (string[len] != '\0'){
		len++;
	}
    return len;
}
