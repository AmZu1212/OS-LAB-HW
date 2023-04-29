CC = gcc
CCLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = attack_TEST.o
EXEC = attack_TEST.exe
RM = rm -rf *.o *.exe

attack_TEST.exe: $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)

attack_TEST.o: attack_TEST.c magic_api.h
	$(CC) $(CFLAGS) -c attack_TEST.c

clean:
	$(RM)