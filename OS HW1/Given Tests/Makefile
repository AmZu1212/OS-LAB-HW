CC = gcc
CCLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = mytest.o
EXEC = mytest.exe
RM = rm -rf *.o *.exe

mytest.exe: $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)

mytest.o: mytest.c magic_api.h
	$(CC) $(CFLAGS) -c mytest.c

clean:
	$(RM)