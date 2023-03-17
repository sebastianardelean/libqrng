CC=gcc
CFLAGS=-Wall -c -g -fPIC -Wno-parentheses -fno-strict-aliasing
LFLAGS=-shared -Wl,-soname,-lcurl
SRC=qrng.c
COMPILE=$(patsubst %.c, %.o, $(SRC))
OBJ=$(wildcard bin/*.o)

SO_OBJ=libqrng

SO_FILE=$(SO_OBJ).so.1.0



all: create_dir $(COMPILE) link

copy_objects:
	mv *.o bin/

create_dir:
	mkdir -p bin

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

link: copy_objects
	$(CC) $(LFLAGS) $(OBJ) -o bin/$(SO_FILE)

install:
	cp bin/$(SO_FILE) /usr/lib
	ln -sf /usr/lib/$(SO_FILE) /usr/lib/$(SO_OBJ).so.1
	ln -sf /usr/lib/$(SO_FILE) /usr/lib/$(SO_OBJ).so
	ln -sf /usr/lib/$(SO_FILE) /usr/lib/$(SO_OBJ).o.1

clean:
	rm -rf bin/

exec:
	$(CC) -Wall -g main.c -o main -lcurl -lqrng
