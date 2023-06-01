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
	start_time = time(NULL);
        sleep(10);
	end_time = time(NULL);
	printf("child is done sleeping\n");
	printf("child slept for %ld time\n", end_time-start_time);
	int ticker = 100;
	while (ticker !=0){
		ticker--;
		printf("ticker is : %d\n", ticker);
	}
        printf("child is done running\n");
        return 0;
    }
    // In parent: sleep for 1 seconds, wake up after 9 or 10
    printf("parent going to sleep\n");
    start_time = time(NULL);
    sleep(1);
    end_time = time(NULL);
    printf("parent woke up\n");
	printf("parent was back after %ld time\n", end_time-start_time);
    // Make sure polling time was as expected
	//assert(end_time - start_time > 8);
	//assert(end_time - start_time < 11);
	printf("Test Done\n");
	
	return 0;
}
