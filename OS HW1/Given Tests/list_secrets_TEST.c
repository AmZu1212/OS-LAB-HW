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
	char secrets[NUM_SECRET][SECRET_MAXSIZE];
	char sodot[NUM_SECRET][SECRET_MAXSIZE];
	// create child processes
	for (i = 0; i < NUM_CHILDREN; i++) {
		pids[i] = fork();
		if (pids[i] == 0) {
			// child process
			printf("===============================================\n");
			printf("Child process %d started (PID: %d)\n", i, getpid());
			switch (i) {
				case 0:// PROCESS 0
					printf("trying to run list from a process not playing\n");
					printf("Process %d transfering to secrets but it cannot play\n", i);
					r = magic_list_secrets(secrets, NUM_SECRET);
					printf("return flag r = %d \n", r);

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
					printf("return flag r = %d \n", r);

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
					printf("return flag r = %d \n", r);

					printf("child %d stealing from process %d \n", i, pids[2]);
					r = magic_legilimens(pids[2]);
					printf("return flag r = %d \n", r);

					//=======================================================================
					printf("TESTING: magic_list_Secret()\n");
					//=======================================================================
					printf("==============================================\n");
					printf("checking for size = 0 && secrets = NULL\n");
					printf("Process %d transfering size = 0 to NULL array\n", i);
					r = magic_list_secrets(NULL, 0);
					printf("r SHOULD BE LIST_LEN...\n");
					printf("r = %d\n", r);
					printf("==============================================\n");
					//=======================================================================
					printf("trying to fall in array = NULL\n");
					printf("Process %d transfering to NULL array\n", i);
					r = magic_list_secrets(NULL, NUM_SECRET);
					if (r == -1) {
						printf("magic_legilimens() failed [SUCCESS] for process %d\n", i);
					}
					else {
						printf("magic_legilimens() succeded [FAIL] for process %d\n", i);
					}
					printf("==============================================\n");
					//=======================================================================
					printf("trying to give size 0\n");
					printf("Process %d transfering to secrets[] but size = 0\n", i);
					r = magic_list_secrets(secrets, 0);
					printf("return flag r = %d \n", r);
					printf("==============================================\n");
					//=======================================================================
					printf("trying to copy less than max\n");
					printf("Process %d transfering to secrets[] but size < actual size\n", i);
					r = magic_list_secrets(secrets, 1);
					printf("return flag r = %d \n", r);
					printf("==============================================\n");
					//=======================================================================
					printf("trying to copy more than max\n");
					printf("Process %d transfering to sodot[] but size >> actual size\n", i);
					r = magic_list_secrets(sodot, NUM_SECRET);
					printf("return flag r = %d \n", r);
					printf("==============================================\n");
					//=======================================================================

					//GAME END PRINT
					printf("PRINTING SECRETS\n");
					printf("(ignore trash prints due to for i>>secrets)\n");

					int j;
					for (j = 0; j < 2; j++) {
						printf("secret number %d : %s \n",j,secrets[j]);
					}
					printf("==============================================\n");
					printf("PRINTING SODOT\n");
					for (j = 0; j < NUM_SECRET; j++) {
						printf("sodot number %d : %s \n",j,sodot[j]);
					}
					printf("==============================================\n");


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

