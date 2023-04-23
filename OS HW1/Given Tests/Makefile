CC = gcc
CCLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = hw1_test.o
EXEC = tester.exe
RM = rm -rf *.o *.exe

tester.exe: $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)

py_magic.o: py_magic.c magic_api.h
	$(CC) $(CFLAGS) -c py_magic.c

hw1_test.o: hw1_test.c magic_api.h
	$(CC) $(CFLAGS) -c hw1_test.c

clean:
	$(RM)