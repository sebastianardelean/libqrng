CC=gcc
CFLAGS = -Wall -extra -c -g -O -DDEBUG
LDFLAGS=-lcurl

OUTDIR = bin

OBJ=qrng


all: createdir compile link

compile:
	$(CC) $(CFLAGS) $(OBJ).c -o $(OUTDIR)/$(OBJ).o

link:
	$(CC) $(OUTDIR)/$(OBJ).o -o $(OUTDIR)/$(OBJ) $(LDFLAGS)

clean:
	rm -r -f $(OUTDIR)/

createdir:
	mkdir -p $(OUTDIR)

