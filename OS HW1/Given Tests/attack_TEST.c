#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include "magic_api.h"
#define NUM_CHILDREN 4
#define NUM_SECRET 5

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
				break;

			case 1:// PROCESS 1
				r = magic_get_wand(11, "secret1");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
				}

				break;

			case 2:// PROCESS 2
				r = magic_get_wand(12, "secret2");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
				}


				break;

			case 3:// PROCESS 3
				r = magic_get_wand(13, "secret3");
				if (r == 0) {
					printf("magic_get_wand() succeeded for process %d\n", i);
				}
				else {
					printf("magic_get_wand() failed for process %d\n", i);
				}


				//=======================================================================
				printf("TESTING: magic_attack()\n");
				printf("TESTING: attack a non player\n");
				r = magic_attack(pids[0]);
				if (r == -1) {
					printf("failed [SUCCESS] r = %d\n", r);
				}
				else {
					printf("succeeded [FAIL] r = %d\n", r);
				}

				//=======================================================================
				printf("TESTING: attack a normal player\n");
				r = magic_attack(pids[1]);
				if (r >= 0) {
					printf("succeeded [SUCCESS] r = %d\n", r);
				}
				else {
					printf("failed [FAIL] r = %d\n", r);
				}
				//=======================================================================
				printf("TESTING: steal from process, then attack the same process\n");
				r = magic_legilimens(pids[2]);
				if (r == 0) {
					printf("magic_legilimens() succeded for process %d\n", i);
				}
				else {
					printf("magic_legilimens() failed for process %d\n", i);
				}
				printf("attacking the same process now:\n");
				r = magic_legilimens(pids[2]);

				if (r == -1) {
					printf("failed [SUCCESS] for process %d\n", i);
				}
				else {
					printf("succeded [FAIL] for process %d\n", i);
				}
				//=======================================================================
				printf("TESTING: attack yourself\n");
				r = magic_attack(pids[2]+1);
				if (r == -1) {
					printf("failed [SUCCESS] r = %d\n", r);
				}
				else {
					printf("success [FAIL] r = %d\n", r);
				}
				//=======================================================================
				printf("TESTING: attack non existant\n");
				r = magic_attack(9999);
				if (r == -1) {
					printf("failed [SUCCESS] r = %d\n", r);
				}
				else {
					printf("success [FAIL] r = %d\n", r);
				}
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
