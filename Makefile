CC=gcc
CFLAGS=-I.
DEPS = align_info.h
OBJ = align_info.o align.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

align: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.c align
