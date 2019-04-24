//Hry
void resumeProcess (int segment, int *result) 
{
    struct PCB *pcb;
    
    setKernelDataSegment();
    pcb = getPCBOfSegment(segment);
    if (pcb == NULL && pcb->state != PAUSED) {
        *result = NOT_FOUND;
    }
    else {
        pcb->state = READY;
        addToReady(pcb);
        *result = SUCCESS;
    }
    restoreDataSegment();
    
}

//Hry
void killProcess (int segment, int *result) 
{
    struct PCB *pcb;

    setKernelDataSegment();
    pcb = getPCBOfSegment(segment);
    if (pcb == NULL) {
        *result = NOT_FOUND;
    }
    else {
        releaseMemorySegment(pcb->segment);
        releasePCB(pcb);
        *result = SUCCESS;
    }
    restoreDataSegment();
}

//Hry
void writeln(char *string){
	printString(string);
	printString("\n\r");
}

//Hry
char compare(char * arr1, char * arr2, int length) 
{
    int idx = 0;
    char equal = 1;
    while ((equal) && (idx < length)) {
        if (arr1[idx] == arr2[idx]) {
            equal = 1;
        }else{
            equal = 0;
        }   
        if (equal && arr1[idx] == '\0') {
            idx = length;
        }
        idx++;
    }
    return equal;
}
