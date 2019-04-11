#ifndef FUNC_H
#define FUNC_H

#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 32
#define MAX_FILENAME 15
#define MAX_SECTORS 16
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 0x100
#define DIR_SECTOR 0x101
#define DIRS_SECTOR 0x101
#define FILE_SECTOR 0x102
#define FILES_SECTOR 0x102
#define SECTOR_SECTOR 0x103
#define SECTORS_SECTOR 0x103
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define ALREADY_EXIST -2
#define ALREADY_EXISTS -2
#define INSUFFICIENT_ENTRIES -3
#define EMPTY 0x00
#define USED 0xFF
#define SUCCESS 0
#define NULL ((void*)0)
#define ARGS_SECTOR 512

void readStringFull(char *string);

void writeln(char *string);

int strcmp(char *s1, char *s2, int length);

void printString(char *string);

void readString(char *string);

void readSector(char *buffer, int sector);

void writeSector(char *buffer, int sector);

void readFile(char *buffer, char *filename, int *success, char parentIndex);

void writeFile(char *buffer, char *filename, int *sectors, char parentIndex);

void deleteFile(char *path, int *result, char parentIndex);

void deleteDirectory(char *path, int *result, char parentIndex);

void executeProgram(char *filename, int segment, int *success, char p);

void terminateProgram(int *result);

void makeDirectory(char *path, int *result, char parentIndex);

void putArgs(char curdir, char argc, char **argv);

void getCurdir(char *curdir);

void getArgc(char *argc);

void getArgv(char index, char *argv);

void println(char *s);

void findDirectory(char *path, char *result, char parentIndex, int *attempt);

void findFile(char *path, char *result, char parentIndex, int *attempt);

void printDirectory(char curdir);

#endif
