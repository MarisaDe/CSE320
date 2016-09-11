# Define (atleast) the following targets: all, clean
# all must build the executable file named mapreduce.
# clean must remove all object files and executable files created.
# Look at the introduction doc for information on how make a Makefile.
# This make file must build according to the structure described in the
# introduction doc, if it doesn't you will get a ZERO!

IDIR =./include/
ISRC = ./src/
CFLAGS=-Wall -Werror
HEAD = map_reduce.h
DEP = -I$(IDIR)
BINS = map_reduce main
COMP = gcc $(CFLAGS) -c $(ISRC)$@.c -I include -o ./build/$@.o
FOLDERS = build bin

#############################################

all: clean make_folders link

make_folders:
	mkdir $(FOLDERS)


link: compile 
	  gcc ./build/** -o ./bin/mapreduce -lm

compile: main map_reduce

main: 
	$(COMP)

map_reduce:
	$(COMP) 


.PHONY: clean

clean:
	rm -rf $(FOLDERS)  
