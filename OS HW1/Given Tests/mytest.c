#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include "magic_api.h"
#define NUM_CHILDREN 3

int main() {
    pid_t pids[NUM_CHILDREN];
    int status;
    int i =0 ;
    int r ;

    // create child processes
    for ( i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // child process
            printf("Child process %d started (PID: %d)\n", i + 1, getpid());
	    
	    r =magic_get_wand(10, "1234567891234567891234567891111111111");
	    printf("first get faile r =%d \n",r);
	    r =magic_get_wand(10, "hii");
	    printf("seconde get success r= %d \n",r);
	    printf("r =%d(success)\n",r);
	    printf("child %d attacking process %d \n",i,pids[0]);
	    //int health = magic_attack(pids[0]);
	    printf("target pid = %d\n",pids[0]);
	    //printf("health = %d\n",health);
            sleep(2);
            printf("Child process %d finished (PID: %d)\n", i + 1, getpid());
            return 0;
        }
    }

    // parent process
    // wait for all child processes to complete
    for ( i = 0; i < NUM_CHILDREN; i++) {
	
        waitpid(pids[i], &status, 0);
    }
    for(i=0;i<3;i++){
	printf("pids[%d]is %d \n",i,pids[i]);
    }
    printf("Parent process finished (PID: %d)\n", getpid());

    return 0;
}
