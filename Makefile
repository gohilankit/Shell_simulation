# Makefile for ush
#
# Vincent W. Freeh
#

CC=gcc
CFLAGS=-g
SRC=parse.c builtins.c main1.c
TARGET=ush

all:
	$(CC) -o $(TARGET) $(SRC)

ush:	$(OBJ)
	$(CC) -o $@ $(OBJ)

clean:
	\rm $(TARGET)


#gcc -o ush parse.c init.c builtins.c main1.c
