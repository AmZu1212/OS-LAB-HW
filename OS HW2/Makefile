CC = gcc
CFLAGS = -Wall -Wextra

# Source file
SRC = hw2_test.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
TARGET = test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
