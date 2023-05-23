CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -c -g -Wno-parentheses -fno-strict-aliasing -I../lib/
LFLAGS=-lcurl -lqrng
SRC=$(wildcard *.c)
COMPILE=$(patsubst %.c, %.o, $(SRC))
OBJ=$(wildcard bin/*.o)

OUT=qrandom


all: create_dir $(COMPILE) link

copy_objects:
	mv *.o bin/

create_dir:
	mkdir -p bin/

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

link: copy_objects
	$(CC) $(LFLAGS) $(OBJ) -o bin/$(OUT)

clean:
	rm -rf bin/
