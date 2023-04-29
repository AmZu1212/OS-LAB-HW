CC = gcc
CCLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = get_wand_TEST.o
EXEC = get_wand_TEST.exe
RM = rm -rf *.o *.exe

get_wand_TEST.exe: $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)

get_wand_TEST.o: get_wand_TEST.c magic_api.h
	$(CC) $(CFLAGS) -c get_wand_TEST.c

clean:
	$(RM)