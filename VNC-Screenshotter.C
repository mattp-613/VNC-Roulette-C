#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h> //O_CREAT | O_EXCL
#include <signal.h>

// SETTINGS
#define MAX_THREADS 400
#define IP_LIST "iplist.txt"
#define VULNERABLE_IP_LIST "iplist_vulnerable.txt"
#define NONVULNERABLE_IP_LIST "iplist_nonvulnerable.txt"
#define IP_LEFT "ipleft.txt"

// OPTIONS
#define LABEL_IPS 1

sem_t *thread_sem;
sem_t *write_sem;

FILE *ipListFile = NULL;
FILE *ipLeftFile = NULL;

int create_thread_semaphore() {
    /*
    Creates a named semaphore for limiting the amount of threads. 
    A named semaphore is used over a regular
    semaphore due to the parent to child and child to child communication.
    */
    thread_sem = sem_open("/thread", O_CREAT | O_EXCL, 0644, MAX_THREADS);
    if (thread_sem == SEM_FAILED) {
        perror("Error creating semaphore");
        return 0;
    }
    if (sem_unlink("/thread") == -1) { //This is required to prevent the semaphore from lasting forever
        perror("Error unlinking semaphore");
        return 0;
    }
    return 1;
}

int create_write_semaphore() {
     /*
    Creates a named semaphore for limiting the amount of threads. 
    A named semaphore is used over a regular
    semaphore due to the parent to child and child to child communication.
    */
    write_sem = sem_open("/write", O_CREAT | O_EXCL, 0645, 1);
    if (write_sem == SEM_FAILED) {
        perror("Error creating semaphore");
        return 0;
    }
    if (sem_unlink("/write") == -1) { //This is required to prevent the semaphore from lasting forever
        perror("Error unlinking semaphore");
        return 0;
    }
    return 1;
}

int open_file(FILE **file, const char *filename, const char *mode) {
	/*
	This function opens a file given the file pointer and the name of the file, with the mode of course.
	It is basically fopen with error detection.
	*/
    *file = fopen(filename, mode);
    if (*file == NULL) {
        perror("Error opening file");
        return 0;
    }
    return 1;
}

void close_file(FILE **file) {
	/*
	This function closes a file given the file pointer variable. 
	It is basically fclose with error detection.
	*/
    if (*file != NULL) {
        fclose(*file);
        *file = NULL;
    }
}

void cleanup_semaphores() {
	/*
	This function cleans up the garbage created by the semaphores.
	*/
    sem_close(write_sem);
    sem_close(thread_sem);
}

void cleanup_files(FILE **ipLeftFile, FILE **ipListFile) {
	/*
	This function cleans up the garbage created by the files.
	*/
    close_file(ipLeftFile);
    close_file(ipListFile);
}

void cleanup() {
    // Cleanup function to be called on CTRL+C signal
    cleanup_files(&ipLeftFile, &ipListFile);
    cleanup_semaphores();
    exit(0);
}

void handle_interrupt(int signum) {
	/*
	Handles the CTRL+C interrupt for the script. Garbage collection isn't necessarily required for this script
	but it is good to do it anyways.
	*/
    printf("\n\n-----Interrupt signal received. Ending script...-----\n\n");
	cleanup();
}

void process_ip(const char* ip) {
    char command[96];
    char tail[96];

    printf("Scanning %s\n", ip);

    // TODO: Attempt to connect, if possible, THEN Screenshot with a longer timeout!
    sprintf(command, "timeout 10 vncsnapshot -quiet -rect 0x0-800-600 %s:0 snapshot_%s.jpg", ip, ip);

    if (sem_wait(thread_sem) == -1) {
        perror("Error waiting on semaphore");
        cleanup();
    }

    if (fork() == 0) {
        if (sem_wait(write_sem) == -1) {
            perror("Error waiting on semaphore");
            cleanup();
        }

        sprintf(tail, "tail -n +2 %s > %s.tmp && mv %s.tmp %s", IP_LEFT, IP_LEFT, IP_LEFT, IP_LEFT);
        system(tail);

        if (sem_post(write_sem) == -1) {
            perror("Error releasing semaphore");
            cleanup();
        }

        int timeout = system(command);

        if (timeout != 0) {
            printf("IP %s timed out.\n", ip);
        }

        if (sem_post(thread_sem) == -1) {
            perror("Error releasing semaphore");
            cleanup();
        }

        cleanup();
		exit(0);
    }
}

int main() {
    char command[96];
    char tail[96];
    char ip[17];

    if (!create_thread_semaphore() || !create_write_semaphore()) {
        cleanup_semaphores();
        return 1;
    }

    if (!open_file(&ipListFile, IP_LIST, "r")) {
        cleanup_semaphores();
        return 1;
    }

    if (!open_file(&ipLeftFile, IP_LEFT, "r")) {
        printf("%s not detected. Creating it as a copy of %s...\n", IP_LEFT, IP_LIST);
        sprintf(command, "cp %s %s", IP_LIST, IP_LEFT);
        system(command);
        if (!open_file(&ipLeftFile, IP_LEFT, "r")) {
            cleanup_semaphores();
            return 1;
        }
    }

	signal(SIGINT, handle_interrupt); //CTRL+C garbage cleanup

    while (fgets(ip, 17, ipLeftFile)) {
        ip[strcspn(ip, "\n")] = 0;
		process_ip(ip);
	}

    cleanup_files(&ipLeftFile, &ipListFile);
    cleanup_semaphores();
    return 0;
}