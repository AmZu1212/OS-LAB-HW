CC = gcc
CCLINK = $(CC)
CFLAGS = -g -Wall -std=c99
OBJS = list_secrets_TEST.o
EXEC = list_secrets_TEST.exe
RM = rm -rf *.o *.exe

list_secrets_TEST.exe: $(OBJS)
	$(CCLINK) $(OBJS) -o $(EXEC)

list_secrets_TEST.o: list_secrets_TEST.c magic_api.h
	$(CC) $(CFLAGS) -c list_secrets_TEST.c

clean:
	$(RM)