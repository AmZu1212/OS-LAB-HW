#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "magic_api.h"

int main()
{
	// simple self test
    pid_t cpid, ppid;
	int res = -1;
    time_t start_time, end_time;
    
    ppid = getpid();
    cpid = fork();
    if (cpid == 0) {
        // In child: request more time
        magic_get_wand(1, "secret");
        res = magic_clock(9);
        assert(res == 0); // verify no error
        printf("child is starting to sleep\n");
        sleep(5);
        printf("child is done sleeping\n");
        return 0;
    }
    // In parent: sleep for 1 seconds, wake up after 9 or 10
    start_time = time(NULL);
    sleep(1);
    end_time = time(NULL);
    printf("parent woke up\n");
    // Make sure polling time was as expected
	assert(end_time - start_time > 8);
	assert(end_time - start_time < 11);
	printf("Test Done\n");
	
	return 0;
}
