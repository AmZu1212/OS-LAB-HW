#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include "magic_api.h"
#define NUM_CHILDREN 4
#define NUM_SECRET 3

int main() {
	pid_t pids[NUM_CHILDREN];
	int status;
	int i = 0;
	int r;
	char secrets[NUM_SECRET][SECRET_MAXSIZE];

	// create child processes
	for (i = 0; i < NUM_CHILDREN; i++) {
		pids[i] = fork();
		if (pids[i] == 0) {
			// child process
			printf("===============================================\n");
			printf("Child process %d started (PID: %d)\n", i, getpid());
			switch (i) {
				case 0:// PROCESS 0
					r = magic_get_wand(10, "secret0");
					if (r == 0) {
						printf("magic_get_wand() succeeded for process %d\n", i);
					}
					else {
						printf("magic_get_wand() failed for process %d\n", i);
					}




					break;

				case 1:// PROCESS 1
					r = magic_get_wand(10, "secret1");
					if (r == 0) {
						printf("magic_get_wand() succeeded for process %d\n", i);
					}
					else {
						printf("magic_get_wand() failed for process %d\n", i);
					}


					printf("child %d stealing from process %d \n", i, pids[0]);
					r = magic_legilimens(pids[0]);




					break;

				case 2:// PROCESS 2
					r = magic_get_wand(10, "secret2");
					if (r == 0) {
						printf("magic_get_wand() succeeded for process %d\n", i);
					}
					else {
						printf("magic_get_wand() failed for process %d\n", i);
					}


					printf("child %d stealing from process %d \n", i, pids[0]);
					r = magic_legilimens(pids[0]);
					printf("return flag r = %d \n", r);

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


					printf("child %d stealing from process %d \n", i, pids[0]);
					r = magic_legilimens(pids[0]);
					printf("return flag r = %d \n", r);

					printf("child %d stealing from process %d \n", i, pids[1]);
					r = magic_legilimens(pids[1]);
					printf("return flag r = %d \n", r);

					printf("child %d stealing from process %d \n", i, pids[2]);
					r = magic_legilimens(pids[2]);
					printf("return flag r = %d \n", r);

					printf("Activating magic_list_Secret()\n");
					printf("Process %d transfering to secrets[] array\n",i);
					r = magic_list_secrets(secrets, NUM_SECRET);
					printf("return flag r = %d \n", r);


					//GAME END PRINT
					int j;
					for (j = 0; j < NUM_SECRET; j++) {
						printf("secret number %d : %s \n",j,secrets[j]);
					}




					break;
			}
			


			printf("===============================================\n");
			sleep(2);
			printf("Child process %d finished (PID: %d)\n", i, getpid());
			return 0;
		}
	}

	//THIS IS OLD MAGIC ATTACK TEST
		//r = magic_legilimens(pids[0]);
		//printf("r = %d(success)\n",r);
		//int health = magic_attack(c);
		//printf("target pid = %d\n",pids[0]);
		//printf("health = %d\n",health);
	//========================================
	 
	 
	
	// parent process [ DONT TOUCH THIS ]
	// wait for all child processes to complete
	for (i = 0; i < NUM_CHILDREN; i++) {

		waitpid(pids[i], &status, 0);
	}
	for (i = 0; i < 3; i++) {
		printf("pids[%d]is %d \n", i, pids[i]);
	}
	printf("Parent process finished (PID: %d)\n", getpid());

	return 0;
}
