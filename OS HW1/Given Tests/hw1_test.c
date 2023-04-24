#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "magic_api.h"

int my_strnlen(const char* str, int max_len) {
    int len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}

int main()
{
	int res = -1;
    pid_t pid = getpid();
    char secrets[5][SECRET_MAXSIZE];

    // Simple test that uses the system calls
	res = magic_get_wand(1, "secret");
	assert(res == 0);

    res = magic_attack(pid);
    assert(res == -1);
    assert(errno == ECONNREFUSED);

    res = magic_legilimens(pid);
    assert(res == 0);

    res = magic_list_secrets(secrets, 5);
    assert(res == 0);
    int length = strlen(secrets[0]);//
    printf("length is: %d\n", length);//
    assert(my_strnlen(secrets[0], SECRET_MAXSIZE) == 0);

	return 0;
}
