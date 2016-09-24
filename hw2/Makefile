IDIR =./include/
ISRC = ./src/
DEP = -I$(IDIR)
BIN = utfconverter
CFLAGS = -g -Wall -Werror -pedantic -Wextra 
FOLDERS = build bin

all: clean make_folders execute

make_folders:
	mkdir $(FOLDERS)

execute: utfconverter
	gcc $(CFLAGS) ./build/** -o ./bin/utf

utfconverter:
	gcc -c $(CFLAGS) $(ISRC)$@.c -I include -o ./build/$@.o

debug: 
	gcc $(CFLAGS) $(ISRC)utfconverter.c -I include -o ./build/utfconverter.o
	gdb ./build/utfconverter.o


.PHONY: clean

clean:
	rm -rf $(FOLDERS)