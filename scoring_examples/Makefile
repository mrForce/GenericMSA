CC=gcc
CFLAGS=-I. -lm
DEPS = points_distance.h
OBJ = points_distance.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

align: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o points_distance
