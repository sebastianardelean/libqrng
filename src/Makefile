CC=gcc
FLAGS=
CFLAGS=-Wall -c -g -fPIC -Wno-parentheses -fno-strict-aliasing $(FLAGS)
LFLAGS=-shared -Wl,-soname,libqrng
SRC=$(wildcard *.c)
COMPILE=$(patsubst %.c, %.o, $(SRC))
OBJ=$(wildcard ../lib/*.o)

SO_OBJ=libqrng

SO_FILE=$(SO_OBJ).so.1.0



all: create_dir $(COMPILE) link

copy_objects:
	mv *.o ../lib/

create_dir:
	mkdir -p ../lib

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

link: copy_objects
	$(CC) $(LFLAGS) $(OBJ) -lcurl -o ../lib/$(SO_FILE)
	rm ../lib/*.o

install:
	cp ../lib/$(SO_FILE) /usr/local/lib
	ln -sf /usr/local/lib/$(SO_FILE) /usr/local/lib/$(SO_OBJ).so.1
	ln -sf /usr/local/lib/$(SO_FILE) /usr/local/lib/$(SO_OBJ).so
	ln -sf /usr/local/lib/$(SO_FILE) /usr/local/lib/$(SO_OBJ).o.1

clean:
	rm -f ../lib/*.*


