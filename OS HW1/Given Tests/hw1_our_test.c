#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "magic_api.h"
#include <assert.h>
#include <sys/wait.h>
#include <string.h>
#include "magic_api.h"

#define NUM_PROCESSES 1 // change to 5


// struct secrets_list for refrence
struct secrets_list {
    struct list_head list;
    char secret[SECRET_MAXSIZE];
};

// strnlen() maybe not needed
int my_strnlen(const char* str, size_t max_len) {
    int len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}


// Helper function to print a process's information
void print_process_info(int pid)
{
    struct task_struct* process = find_task_by_vpid(pid);
    printf("Process %d:\n", pid);
    printf("\tHolding wand: %d\n", process->holding_wand);
    printf("\tHealth: %d\n", process->health);
    printf("\tPower: %d\n", process->power);
    printf("\tSecret: %s\n", process->my_secret);
}

int main()
{
    // Initialize some processes with random power levels and secrets
    int powers[NUM_PROCESSES] = { 10 }; //, 20, 30, 40, 50 };
    char* secrets[NUM_PROCESSES] = { "secret1" }; //, "secret2", "secret3", "secret4", "secret5" };
    pid_t pids[NUM_PROCESSES];
    int i;

    // Initialize parent for playing
    char* parent_secret = "parent's_secret";
    int res = magic_get_wand(11, parent_secret);
    if (res < 0) {
        fprintf(stderr, "Error: Could not get wand for parent process %d\n", getpid());
        return 1;
    }
    
    // test for double init on parent
    res = magic_get_wand(90, parent_secret);
    assert(res == -1);
    assert(errno == EEXIST);


    // Initialize child for playing (magic_get_wand())
    for (i = 0; i < NUM_PROCESSES; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return 1;
        }
        else if (pids[i] == 0) {
            // Child process
            res = magic_get_wand(powers[i], secrets[i]);
            if (res < 0) {
                fprintf(stderr, "Error: Could not get wand for process %d\n", getpid());
                //assert(errno == ENOMEM);
                return 1;
            }
            printf("Process %d got a wand with power %d and secret \"%s\"\n", getpid(), powers[i], secrets[i]);
            while (1) {
                // Child process just loops until it's attacked
            }
        }
        else {
            // Parent process
            printf("Initialized process %d with power %d and secret \"%s\"\n", pids[i], powers[i], secrets[i]);
        }
    }


    // Parent process attacks each child process once (magic_attack())
    for (i = 0; i < NUM_PROCESSES; i++) {
        printf("Parent process attacking child process %d\n", pids[i]);
        int res = magic_attack(pids[i]);
        if (res < 0) {
            fprintf(stderr, "Error: Could not attack process %d, errno=%d\n", pids[i], errno);
        }
        else {
            printf("Attack successful! New health of process %d is %d\n", pids[i], res);
            print_process_info(pids[i]);
        }
    }


    // Parent process steals each child process's secret once (magic_legilimens())
    for (i = 0; i < NUM_PROCESSES; i++) {
        printf("Parent process stealing secret from child process %d\n", pids[i]);
        int res = magic_legilimens(pids[i]);
        if (res < 0) {
            fprintf(stderr, "Error: Could not steal secret from process %d, errno=%d\n", pids[i], errno);
        }
        else {
            printf("Secret stolen successfully!\n");
            // Print the stolen secret
            struct task_struct* current_process = current;
            struct list_head* secrets = &(current_process->secrets_ptr);
            struct list_head* pos;
            struct secrets_list* curr_secret;
            list_for_each(pos, secrets) {
                curr_secret = list_entry(pos, struct secrets_list, list);
                printf("Stolen secret: %s\n", curr_secret->secret);
            }
        }
    }

    // Parent process adds a secret to the child process's secret list (magic_secrets_list())
    for (i = 0; i < NUM_PROCESSES; i++) {
        printf("Parent process adding secret to child process %d\n", pids[i]);
        char new_secret[SECRET_MAXSIZE] = "new_secret";
        int res = magic_secrets_list(pids[i], new_secret);
        if (res < 0) {
            fprintf(stderr, "Error: Could not add secret to process %d, errno=%d\n", pids[i], errno);
        }
        else {
            printf("Secret added successfully!\n");
            // Print the updated secret list
            struct task_struct* current_process = current;
            struct list_head* secrets = &(current_process->secrets_ptr);
            struct list_head* pos;
            struct secrets_list* curr_secret;
            printf("Current secrets list for process %d:\n", pids[i]);
            list_for_each(pos, secrets) {
                curr_secret = list_entry(pos, struct secrets_list, list);
                printf("\t%s\n", curr_secret->secret);
            }
        }
    }
    printf("testing done, exiting...\n");
    return 0;
}