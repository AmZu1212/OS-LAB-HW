CC = gcc
CFLAGS = -Wall

# Object files
OBJ = hw2_test.o

test: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o test

hw2_test.o: magic_api.h
	$(CC) $(CFLAGS) -c hw2_test.c -o $(OBJ)

clean:
	rm -f *.o *.exe test
