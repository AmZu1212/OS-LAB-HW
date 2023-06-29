#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ioctl.h>

// Globals
#define DEVICE_PATH "/dev/repeated"


// Utilities for calculating the IOCTL command codes
#define _IOC_NRBITS   8
#define _IOC_TYPEBITS 8
#define _IOC_SIZEBITS 14
#define _IOC_DIRBITS  2

#define _IOC_NRMASK   ((1 << _IOC_NRBITS) - 1)
#define _IOC_TYPEMASK ((1 << _IOC_TYPEBITS) - 1)
#define _IOC_SIZEMASK ((1 << _IOC_SIZEBITS) - 1)
#define _IOC_DIRMASK  ((1 << _IOC_DIRBITS) - 1)

#define _IOC_NRSHIFT   0
#define _IOC_TYPESHIFT (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT  (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC_NONE 0
#define _IOC_WRITE 1
#define _IOC_READ 2

#define _IOC(dir, _type, nr, size) \
	((((dir)  << _IOC_DIRSHIFT) | \
	((_type) << _IOC_TYPESHIFT) | \
	((nr)   << _IOC_NRSHIFT) | \
	((size) << _IOC_SIZESHIFT)))

#define _IO(_type, nr) _IOC(_IOC_NONE, _type, nr, 0)
#define _IOR(_type, nr, size) _IOC(_IOC_READ, _type, nr, sizeof(size))
#define _IOW(_type, nr, size) _IOC(_IOC_WRITE, _type, nr, sizeof(size))


sem_t sem_A, sem_B;

void process_A()
{
	printf("process A start (1) \n");
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Error opening the device file");
        exit(1);
    }

    // SET_STRING "ABC"
    char repeated[] = "ABC";
    int cmd_number = 0; // Specify the appropriate IOCTL command number
    if (ioctl(fd, cmd_number, repeated) == -1) {
        perror("Error writing to device");
        close(fd);
        exit(1);
    }

    // Write "Hello"
    char message[] = "Hello";
    if (write(fd, message, strlen(message)) == -1) {
        perror("Error writing to device");
        close(fd);
        exit(1);
    }
	printf("process A end (1) \n");
    sem_post(&sem_B); // Signal Process B to read

    sem_wait(&sem_A); // Wait for Process B to read 3 bytes

	printf("process A start (3) \n");
    // SET_STRING "Aha"
    char repeated_new[] = "Aha";
    if (ioctl(fd, cmd_number, repeated_new) == -1) {
        perror("Error writing to device");
        close(fd);
        exit(1);
    }

	printf("process A end (3) \n");
    sem_post(&sem_B); // Signal Process B to continue reading

	printf("process A start (5) \n");
    // Read 5 bytes
    char buffer[6];
    ssize_t bytesRead = read(fd, buffer, 5);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process A read: %s\n", buffer);
	printf("process A end (5) \n");
    close(fd);
}

void process_B()
{
    sem_wait(&sem_B); // Wait for Process A to write
	printf("process B start (2) \n");
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Error opening the device file");
        exit(1);
    }

    // Read 3 bytes
    char buffer[4];
    ssize_t bytesRead = read(fd, buffer, 3);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process B read: %s\n", buffer);

	printf("process B end (2) \n");
    sem_post(&sem_A); // Signal Process A to read 5 bytes

    sem_wait(&sem_B); // Wait for Process A to execute SET_STRING "Aha"

	printf("process B start (4) \n");
    // Read 3 bytes
    bytesRead = read(fd, buffer, 3);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process B read: %s\n", buffer);
	
	printf("process B end (4) \n");
    sem_wait(&sem_B); // Wait for Process A to read 5 bytes

	printf("process B start last (6) \n");
    // llseek -10
    if (lseek(fd, -10, SEEK_CUR) == -1) {
        perror("Error seeking device file");
        close(fd);
        exit(1);
    }
	
    // Read 8 bytes
    bytesRead = read(fd, buffer, 8);
    if (bytesRead == -1) {
        perror("Error reading from device");
        close(fd);
        exit(1);
    }

    buffer[bytesRead] = '\0';
    printf("Process B read: %s\n", buffer);
	
	printf("process B end last (6) \n");
    close(fd);
}

int main()
{
    sem_init(&sem_A, 0, 0);
    sem_init(&sem_B, 0, 0);

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

    sem_destroy(&sem_A);
    sem_destroy(&sem_B);

    return 0;
}
