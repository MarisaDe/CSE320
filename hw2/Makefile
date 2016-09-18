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
	gcc ./build/** -o ./bin/utfconverter

utfconverter:
	gcc -c $(ISRC)$@.c -I include -o ./build/$@.o

.PHONY: clean

clean:
	rm -rf $(FOLDERS)

