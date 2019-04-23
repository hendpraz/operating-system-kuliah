#!/bin/sh
set -e

# "Menyiapkan file..."
dd if=/dev/zero of=floppya.img bs=512 count=2880
nasm bootload.asm
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=map.img of=floppya.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=floppya.img bs=512 count=1 seek=258 conv=notrunc
dd if=sectors.img of=floppya.img bs=512 count=1 seek=259 conv=notrunc

# "Kompilasi kernel..."
bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=1 iflag=fullblock
as86 lib.asm -o lib_asm.o

# "Kompilasi fungsi-fungsi pembantu..."
bcc -ansi -c -o helper.o helper.c

# "Kompilasi shell dan fitur utilitas..."
bcc -ansi -c shell.o shell.c
ld86 -o shell -d shell.o lib_asm.o helper.o

bcc -ansi -c ls.o ls.c
ld86 -o ls -d ls.o lib_asm.o helper.o

bcc -ansi -c mkdir.o mkdir.c
ld86 -o mkdir -d mkdir.o lib_asm.o helper.o

bcc -ansi -c cat.o cat.c
ld86 -o cat -d cat.o lib_asm.o helper.o

bcc -ansi -c rm.o rm.c
ld86 -o rm -d rm.o lib_asm.o

bcc -ansi -c echo.o echo.c
ld86 -o echo -d echo.o lib_asm.o

#loadFile hasil kompilasi
./loadFile shell
./loadFile ls
./loadFile echo
./loadFile mkdir
./loadFile rm
./loadFile cat
./loadFile keyproc2

#For debug
./loadFile cattest.txt

bochs -f opsys.bxrc

