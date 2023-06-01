#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h> //O_CREAT


//SETTINGS
#define MAX_THREADS 1 // maximum number of forks allowed (counting from 0)
#define IP_LIST "iplist.txt"
#define VULERNABLE_IP_LIST "iplist_vulnerable.txt"
#define NONVULERNABLE_IP_LIST "iplist_nonvulnerable.txt"
#define IP_LEFT "ipleft.txt"


//OPTIONS
#define LABEL_IPS 1 //Should there be ip list of vulrenable and/or non vulnerable IP's created?


sem_t *thread_sem; // semaphore for limiting forks
sem_t *write_sem; // semaphore for limiting file writing (a file lock)

int main() {
	char ip[17];
	FILE *fp;
	FILE *fs;

    thread_sem = sem_open("/thread", O_CREAT | O_EXCL, 0644, MAX_THREADS); //Named semaphore required for parent-child intercommunication

	if (thread_sem == SEM_FAILED) {
        perror("Error creating semaphore");
        return 1;
    }

	if (sem_unlink("/thread") == -1) { //This is required to prevent the semaphore from lasting forever
        perror("Error unlinking semaphore");
        return 1;
    }

	//Writing to file semaphore
	//This is to ensure only one thread writes to a file at a time!
	
	write_sem = sem_open("/write", O_CREAT | O_EXCL, 0645, 1); //Named semaphore required for parent-child intercommunication

	if (write_sem == SEM_FAILED) {
        perror("Error creating semaphore");
        return 1;
    }

	if (sem_unlink("/write") == -1) { //This is required to prevent the semaphore from lasting forever
        perror("Error unlinking semaphore");
        return 1;
    }

	fp = fopen(IP_LIST, "r");
	if (fp == NULL) {
		perror("Error opening file");
		return 1;
	}

	fs = fopen(IP_LEFT, "r");
	if (fs == NULL) {
		//TODO: Copy IP_LIST To a new file IP_LEFT
		}

	while(fgets(ip, 17, fp)) {
		printf("Scanning %s",ip);
		ip[strcspn(ip, "\n")] = 0; // remove newline character

		char command[69];
		sprintf(command, "timeout 10 vncsnapshot -quiet -rect 0x0-800-600 %s:0 snapshot_%s.jpg", ip, ip);
		if (sem_wait(thread_sem) == -1) {
            perror("Error waiting on semaphore");
            return 1;
        }

		if(fork() == 0) {
			
			if (sem_wait(write_sem) == -1) {
            perror("Error waiting on semaphore");
            return 1;
        	}

			//TODO: remove ip from ipsleft.txt
			
			if (sem_post(write_sem) == -1) {
                perror("Error releasing semaphore");
                return 1;
            }

			int timeout = system(command);

			if(timeout != 0) {
    			printf("IP %s timed out. Adding to %s\n",ip, NONVULERNABLE_IP_LIST);
				//TODO: add to nonvulnerable txt here
			}

			else{
				printf("IP %s successful. Screenshot taken. Adding to %s\n",ip, VULERNABLE_IP_LIST);
				//TODO: add to vulnerable txt here
			}

			if (sem_post(thread_sem) == -1) {
                perror("Error releasing semaphore");
                return 1;
            }

			sem_close(thread_sem); //TODO: Move fclose, semclose, etc to a CTRL+C exception
			sem_close(write_sem);
			exit(0);

		}

	}

	fclose(fp);
	fclose(fs);
	return 0;
}
