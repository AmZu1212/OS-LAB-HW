#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include "magic_api.h"
#define NUM_CHILDREN 3
#define NUM_SECRET 2

int main() {
    pid_t pids[NUM_CHILDREN];
    int status;
    int i =0 ;
    int r ;
    char secrets[NUM_SECRET][SECRET_MAXSIZE];

    // create child processes
    for ( i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // child process
            printf("Child process %d started (PID: %d)\n", i + 1, getpid());
	    switch(i){
	    case 0:
	    	r =magic_get_wand(10, "secret0");
		break;
	    case 1:
	    	r =magic_get_wand(10, "secret1");
		break;
	    case 2:
	    	r =magic_get_wand(10, "secret2");
		break;
	    }

	    printf("r = %d(success)\n",r);
	    printf("child %d stealing from process %d \n",i,pids[0]);
	    r = magic_legilimens(pids[0]);
	    printf("r = %d(success)\n",r);

	    r = magic_list_secrets(secrets, NUM_SECRET);
	    printf("r = %d\n",r);
	
	    for(i=0;i<NUM_SECRET;i++){
		printf("secret number %d : %s \n",i,secrets[i]);
	    }

            //r = magic_legilimens(pids[0]);
	    //printf("r = %d(success)\n",r);
	    //int health = magic_attack(c);
	    //printf("target pid = %d\n",pids[0]);
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
