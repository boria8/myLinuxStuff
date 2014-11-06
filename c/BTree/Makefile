destination = utestBTree
Debug_OR_Release = release
CFLAGS =  -Wall -ansi -pedantic 
CPPFLAG=-D$(destination)
CC = gcc
COMPILE_ONLY = -c
PUBLIC=src
INCLUDES = -I$(src)
SRC = src
EXE=$(destination)
OBJ=obj
OBJS=$(SRC)/BTree.o
CLIENT=./Client
BIN=bin


Debug:   Debug_OR_Release=Debug
Debug:   CFLAGS+=-g -D_DEBUG
Debug:   SUBDIRS= ./$(OBJ) ./$(BIN)/
Debug:   OBJ_F=$(OBJ)/$(Debug_OR_Release)/BTree.o

Release: Debug_OR_Release=Release
Release: CFLAGS+=-DNDEBUG -O2
Release: SUBDIRS= ./$(OBJ)/$(Debug_OR_Release)/$(src) ./$(BIN)/$(Debug_OR_Release)
Release: OBJ_F=$(OBJ)/$(Debug_OR_Release)/BTree.o

all:Debug Release


Debug:   create_folders  BTreearch  Install 
Release: create_folders  BTreearch  Install 

copyfiles:$(SRC)/BTree.o
	cp $(SRC)/BTree.o bin/debug
	cp public/BTree.h bin/public


BTreearch:  BTree.object
	ar -r -s  ./$(BIN)/$(Debug_OR_Release)/BTree.a  $(OBJS)

BTree.object: $(SRC)/BTree.c $(PUBLIC)/BTree.h
	$(CC)  $(CFLAGS) -I $(PUBLIC)  $(COMPILE_ONLY) $(SRC)/BTree.c  -o $(OBJS)

BTree.pre: $(SRC)/Elephant_list.c
	$(CC)  $(CFLAGS) -I $(PUBLIC) -E $(SRC)/Elephant_list.c  -o $(SRC)/Listwarp.p


create_folders:
	- mkdir -p $(OBJ)/$(Debug_OR_Release)
	- mkdir -p  $(BIN)/$(Debug_OR_Release)


Install:creatInstalFolders  CopyHfile CopyLibFile Clear


CopyHfile:
	- cp $(PUBLIC)/BTree.h $(CLIENT)/public

CopyLibFile:
	- cp ./$(BIN)/$(Debug_OR_Release)/BTree.a $(CLIENT)/$(BIN)/$(Debug_OR_Release)


creatInstalFolders:
	- mkdir -p $(CLIENT)/public
	- mkdir -p $(CLIENT)/$(BIN)/$(Debug_OR_Release)

$(OUT)/$(EXE) : $(OBJS)
	$(CC) -o $(OUT)/$(EXE) $(OBJS)


Clear:
	for i in $(SUBDIRS); do\
		(cd $$i; rm -rf  );\
	done
clean:
	- rm $(SRC)/*.o
