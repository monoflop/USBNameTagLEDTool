LIBS  = -lusb-1.0 -lrt -lpthread
CFLAGS = -Wall -std=c11
SRC=$(wildcard *.c)

ledtool: $(SRC)
	gcc -Iinclude/ -o $@ $^ $(CFLAGS) $(LIBS)
