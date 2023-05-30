CC = gcc
CFLAGS = -Wall

# Object files
OBJ = hw2_test.o

# Executable name
TARGET = test

test: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

hw2_test.o: hw2_test.c magic_api.h
	$(CC) $(CFLAGS) -c hw2_test.c -o $(OBJ)

clean:
	rm -f *.o *.exe test
