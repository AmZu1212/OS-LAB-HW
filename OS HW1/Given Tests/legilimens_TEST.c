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

    // create child processes
    for ( i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // child process
			printf("===============================================\n");
			printf("Child process %d started (PID: %d)\n", i, getpid());
			switch (i) {
			case 0:// PROCESS 0
				//this is an idle process, it does nothing
				break;

			case 1:// PROCESS 1
				r = magic_get_wand(10, "secret1");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
				}

				break;

			case 2:// PROCESS 2
				r = magic_get_wand(10, "secret2");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
				}

				printf("child %d stealing from process %d \n", i, pids[1]);
				r = magic_legilimens(pids[1]);
				if (r == 0) {
					printf("magic_legilimens() succeded for process %d\n", i);
				}
				else {
					printf("magic_legilimens() failed for process %d\n", i);
				}

				break;

			case 3:// PROCESS 3
				r = magic_get_wand(10, "secret3");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
				}

				printf("child %d stealing from process %d \n", i, pids[1]);
				r = magic_legilimens(pids[1]);

				if (r == 0) {
					printf("magic_legilimens() succeded for process %d\n", i);
				}
				else {
					printf("magic_legilimens() failed for process %d\n", i);
				}

				printf("child %d stealing from process %d \n", i, pids[2]);
				r = magic_legilimens(pids[2]);
				if (r == 0) {
					printf("magic_legilimens() succeded for process %d\n", i);
				}
				else {
					printf("magic_legilimens() failed for process %d\n", i);
				}


				//=======================================================================
				printf("TESTING FOR 'TARGET NO WAND'\n");
				r = magic_legilimens(pids[0]);
				if (r == -1) {
					printf("magic_legilimens() failed [SUCCESS] for process %d\n", i);
				}
				else {
					printf("magic_legilimens() succeded [FAIL] for process %d\n", i);
				}
				printf("\n\n");
				//=======================================================================
				printf("TESTING FOR 'READING MYSELF'\n");
				r = magic_legilimens(pids[3]);
				if (r == 0) {
					printf("magic_legilimens() succeded [SUCCESS] for process %d\n", i);
				}
				else {
					printf("magic_legilimens() failed [FAIL] for process %d\n", i);
				}
				printf("\n\n");
				//=======================================================================
				printf("TESTING FOR 'DOUBLE STEAL'\n");
				r = magic_legilimens(pids[2]);
				if (r == -1) {
					printf("magic_legilimens() failed [SUCCESS] for process %d\n", i);
				}
				else {
					printf("magic_legilimens() succeded [FAIL] for process %d\n", i);
				}
				printf("\n\n");
				//=======================================================================
				printf("TESTING FOR 'INVALID PID'\n");
				r = magic_legilimens(-12345);
				if (r == -1) {
					printf("magic_legilimens() failed [SUCCESS] for process %d\n", i);
				}
				else {
					printf("magic_legilimens() succeded [FAIL] for process %d\n", i);
				}
				printf("\n\n");
				//=======================================================================
				break;
			}

			printf("===============================================\n");
			sleep(2);
			printf("Child process %d finished (PID: %d)\n", i, getpid());
			return 0;
		}
	}

	for (i = 0; i < NUM_CHILDREN; i++) {

		waitpid(pids[i], &status, 0);
	}
	for (i = 0; i < 3; i++) {
		printf("pids[%d]is %d \n", i, pids[i]);
	}
	printf("Parent process finished (PID: %d)\n", getpid());

	return 0;
}

