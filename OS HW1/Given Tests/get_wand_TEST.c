#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include "magic_api.h"
#define NUM_CHILDREN 3

int main() {
	pid_t pids[NUM_CHILDREN];
	int status;
	int i = 0;
	int r;

	for (i = 0; i < NUM_CHILDREN; i++) {
		pids[i] = fork();
		if (pids[i] == 0) {
			// child process
			printf("===============================================\n");
			printf("Child process %d started (PID: %d)\n", i, getpid());
			switch (i) {
			case 0:// PROCESS 0
				// idle process
				printf("TESTING: double wand for process %d\n", i);
				r = magic_get_wand(11, NULL);
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
					printf("THIS IS INCORRECT\n");
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
					printf("THIS IS CORRECT\n");
				}
				break;

			case 1:// PROCESS 1
				printf("TESTING: double wand for process %d\n", i);
				r = magic_get_wand(11, "secret1");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
					printf("SUCCESS\n");
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
					printf("FAIL\n");
				}

				r = magic_get_wand(11, "secret1");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
					printf("THIS IS INCORRECT\n");
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
					printf("THIS IS CORRECT\n");
				}


				break;

			case 2:// PROCESS 2
				r = magic_get_wand(12, "\0");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
					printf("THIS IS INCORRECT\n");
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
					printf("THIS IS CORRECT\n");
				}


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