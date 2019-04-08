void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int plus(int a, int b);
int minus(int a, int b);
int divide(int a, int b);
int modulo(int a, int b);
void executeProgram(char *filename, int segment, int *success);

int main(){
	int a, b, temp;
	makeInterrupt21();
	handleInterrupt21(0x0, "Masukkan kedua angka : ", 0, 0);
	readString(a);
	readString(b);
	
	temp = plus(a,b);
	printString(temp);
	temp = minus(a,b);
	printString(temp);
	temp = divide(a,b);
	printString(temp);
	temp = modulo(a,b);
	printString(temp);
	
	handleInterrupt21(0x6, "keyproc", 0x2000, 0);
	while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
	switch (AX) {
		case 0x0:
			printString(BX);
			break;
		case 0x1:
			readString(BX);
			break;
		case 0x2:
			readSector(BX, CX);
			break;
		case 0x3:
			writeSector(BX, CX);
			break;
		case 0x4:
			readFile(BX, CX, DX);
			break;
		case 0x5:
			writeFile(BX, CX, DX);
			break;
		case 0x6:
			executeProgram(BX, CX, DX);
			break;
		default:
			printString("Invalid interrupt");
	}
}

int plus(int a, int b){
	return a + b;
}

int minus(int a, int b){
	return a - b;
}

int divide(int a, int b){
	int q = 0;
	while(q*b <= a) {
		q = q+1;
	}
	return q-1;
}

int modulo(int a, int b){
	while(a >= b) {
		a = a - b;
	}
	return a;
}

void readString(char *string){
	int input;
	int count = 0;
	while(1){
		input = interrupt(0x16,0,0,0,0);
		if (input == '\r'){              
			string[count] = 0x0;
			string[count+1] = '\r';
			string[count+2] = '\n';
			return;
		}
		else if (input == '\b'){
			if (count >= 1){
				string[count] = 0x0;
				interrupt(0x10,0xE00+'\b',0,0,0);
				interrupt(0x10,0xE00+'\0',0,0,0);
				interrupt(0x10,0xE00+'\b',0,0,0);
				count--;
			}
		}
		else{
			string[count] = input;
			interrupt(0x10, 0xE00+input, 0, 0, 0);
			count++;
		}     
	}
}

void printString(char *string){
	int i = 0;
	char c;
	while(string[i] != '\0'){
		c = string[i];
		interrupt(0x10, 0xE00+c, 0, 0, 0);
		i++;
	}
}

void executeProgram(char *filename, int segment, int *success){
	char buffer[MAX_SECTORS*SECTOR_SIZE];
	int i;
	readFile(buffer, filename, success);
	if (*success == 1) {	
		for(i=0; i<MAX_SECTORS*SECTOR_SIZE; i++){
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
}
