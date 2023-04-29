CC = gcc
CCLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = legilimens_TEST.o
EXEC = legilimens_TEST.exe
RM = rm -rf *.o *.exe

legilimens_TEST.exe: $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)

legilimens_TEST.o: legilimens_TEST.c magic_api.h
	$(CC) $(CFLAGS) -c legilimens_TEST.c

clean:
	$(RM)