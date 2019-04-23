#include "helper.h"

int main()
{
    char curdir;
    char argc;
    char *arg;

    getArgc(&argc);
    getCurdir(&curdir);
    if (argc == 1)
    {
        // Argumen hanya 1
        char content[16 * SECTOR_SIZE];
        int success;
        getArgv(0, arg);

        readFile(content, arg, &success, curdir);
        if (success == NOT_FOUND)
        {
            println("File tidak ditemukan");
        }
        else
        {
		writeln("Isi file Anda");
            writeln(content);
        }
    }
    else if (argc == 2)
    {
        //Argumen ada 2
        getArgv(1, arg);
        if (streq(arg, "-w"))
        {
			//Jika argumen terakhir -w, write mode
            char buffer[16 * 512];
            int result;
            int success;
            getArgv(0, arg);
            findFile(arg, &result, curdir, &success);
            if (success == 1)
            {
                writeln(arg);
                println("File sudah Ada!");
            }
            else
            {
                readString(buffer);
                writeFile(buffer, arg, &result, curdir);
                if (result == INSUFFICIENT_SECTORS)
                {
                    writeln("Sector penuh!");
                }
                else if (result == INSUFFICIENT_ENTRIES)
                {
                    writeln("Entry penuh!");
                }
                else if (result == NOT_FOUND)
                {
                    writeln("Direktori tidak ditemukan");
                }
                else if (result == ALREADY_EXIST)
                {
                    println("Terjadi error!");
                }
            }
        }
        else
        {
			//Flag tidak valid
			writeln("Flag tidak valid!");
            writeln("Flag tersedia \"-w\"");
        }
    }
    else
    {
        writeln("Argumen tidak valid!");
        writeln("Isi hanya dengan 2 argumen");
    }
    terminateProgram(0);
}
