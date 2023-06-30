#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

// Globals
#define DEVICE_PATH "/dev/repeated"
#define MY_MAGIC 'r'

#define SET_STRING _IOW(MY_MAGIC,0,int)

void process_A()
{

    printf("Process A starts run\n");

    printf("Process A start (1)\n");
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Error opening the device file in A");
        exit(1);
    }

    // SET_STRING "ABC"
    char repeated[] = "ABC";
    int cmd_number = SET_STRING;
    if (ioctl(fd, cmd_number, repeated) == -1) {
        perror("Error writing to device");
        close(fd);
        exit(1);
    }
    printf("ioctl succeeded!!\n");

    char message[] = "Hello";
    if (write(fd, message, strlen(message)) == -1) {
        perror("Error writing to device");
        close(fd);
        exit(1);
    }
    printf("Process A end (1)\n");

    sleep(1);
    //sem_post(&sem_B); // Signal Process B to read
    //int val ;
    //sem_getvalue(&sem_B,&val);
    //printf("sem_B in A is = %d \n",val);
    //sem_wait(&sem_A); // Wait for Process B to read 3 bytes

    printf("Process A start (3)\n");
    // SET_STRING "Aha"
    char repeated_new[] = "Aha";
    if (ioctl(fd, cmd_number, repeated_new) == -1) {
        perror("Error writing to device");
        close(fd);
        exit(1);
    }

    printf("Process A end (3)\n");
    //sem_post(&sem_B); // Signal Process B to continue reading
    sleep(3);
    printf("Process A start (5)\n");
    char buffer[6];
    ssize_t bytesRead = read(fd, buffer, 5);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process A read: %s\n", buffer);
    printf("Process A end (5)\n");
    sleep(5);
    close(fd);
}

void process_B()
{
    printf("Process B starts run\n");
    //printf("sem_B in B:- sem_B is %d \n",sem_B);
	
    //int val ;
    //sem_getvalue(&sem_B,&val);
    //printf("sem_B IN B is = %d \n",val);

    //sem_wait(&sem_B); // Wait for Process A to write

    printf("Process B start (2)\n");
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Error opening the device file in B");
        exit(1);
    }

    char buffer[4];
    ssize_t bytesRead = read(fd, buffer, 3);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process B read: %s\n", buffer);

    printf("Process B end (2)\n");
    sleep(2);
    //sem_post(&sem_A); // Signal Process A to read 5 bytes

    //sem_wait(&sem_B); // Wait for Process A to execute SET_STRING "Aha"

    printf("Process B start (4)\n");
    bytesRead = read(fd, buffer, 3);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process B read: %s\n", buffer);

    printf("Process B end (4)\n");
    //sem_wait(&sem_B); // Wait for Process A to read 5 bytes
    sleep(4);
    printf("Process B start last (6)\n");
    if (lseek(fd, -10, SEEK_CUR) == -1) {
        perror("Error seeking device file");
        close(fd);
        exit(1);
    }

    bytesRead = read(fd, buffer, 12);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process B read: %s\n", buffer);

    printf("Process B end last (6)\n");
    close(fd);
}

int main()
{
    
    //sem_t sem_A, sem_B;
    //sem_init(&sem_A, 0, 0);
    //sem_init(&sem_B, 0, 0);
    
    //sem_post(&sem_A); // Signal Process B to read
    //printf("in AAA main:- sem_A is %d \n",sem_A);

    pid_t pid = fork();

    if (pid == -1) {
        perror("Error creating child process");
        exit(1);
    } else if (pid == 0) {
        // Child process (Process B)
        process_B();
    } else {
        // Parent process (Process A)

        process_A();
    }

        //sem_destroy(&sem_A);
        //sem_destroy(&sem_B);

    return 0;
}
