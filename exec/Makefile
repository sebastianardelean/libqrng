CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -c -g -Wno-parentheses -fno-strict-aliasing -I../lib/ -DDEBUG
LFLAGS=-lqrng -lcurl
SRC=$(wildcard *.c)
COMPILE=$(patsubst %.c, %.o, $(SRC))
OBJ=$(wildcard bin/*.o)

OUT=qrand


all: create_dir $(COMPILE) link

copy_objects:
	mv *.o bin/

create_dir:
	mkdir -p bin/

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

link: copy_objects
	$(CC) $(OBJ) -o bin/$(OUT) $(LFLAGS)

clean:
	rm -rf bin/
