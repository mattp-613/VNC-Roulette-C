#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h> //O_CREAT

#define MAX_THREADS 10 // maximum number of forks allowed (counting from 0)
sem_t *thread_sem; // semaphore for limiting forks

int main() {
	char ip[17];
	FILE *fp;

	char thread_name[17];
	sprintf(thread_name,"/thread_sem_%d",MAX_THREADS); //To ensure each # of threads is exclusive to their own file
    thread_sem = sem_open(thread_name, O_CREAT | O_TRUNC, 0644, MAX_THREADS); //Named semaphore required for parent-child intercommunication
	if (thread_sem == SEM_FAILED) {
        perror("Error creating semaphore");
        return 1;
    }

	fp = fopen("iplist.txt", "r");
	if (fp == NULL) {
		perror("Error opening file");
		return 1;
	}

	while(fgets(ip, 17, fp)) {
		printf("Scanning %s",ip);
		ip[strcspn(ip, "\n")] = 0; // remove newline character

		char command[96];
		sprintf(command, "timeout 10 vncsnapshot -quiet -rect 0x0-800-600 %s:0 snapshot%s.jpg", ip, ip);

		if (sem_wait(thread_sem) == -1) {
            perror("Error waiting on semaphore");
            return 1;
        }

		if(fork() == 0) {

			int timeout = system(command);

			if(timeout != 0) {
    			printf("IP %s timed out.\n",ip);
			}

			if (sem_post(thread_sem) == -1) {
                perror("Error releasing semaphore");
                return 1;
            }
			sem_close(thread_sem);
			exit(0);

		}

	}

	if (sem_unlink("/thread_sem") == -1) {
        perror("Error unlinking semaphore");
        return 1;
    }
	fclose(fp);
	return 0;
}
