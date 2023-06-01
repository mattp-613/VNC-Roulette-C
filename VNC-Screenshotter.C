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
#define SAVE_IPS 1 //should we use an ip_left list to remove IP's already classified as vulernable/non-vulnerable?

sem_t *thread_sem; // semaphore for limiting forks

int main() {
	char ip[17];
	FILE *fp;


    thread_sem = sem_open("/thread", O_CREAT | O_EXCL, 0644, MAX_THREADS); //Named semaphore required for parent-child intercommunication

	if (thread_sem == SEM_FAILED) {
        perror("Error creating semaphore");
        return 1;
    }

	if (sem_unlink("/thread") == -1) { //This is required to prevent the semaphore from lasting forever
        perror("Error unlinking semaphore");
        return 1;
    }

	fp = fopen(IP_LIST, "r");
	if (fp == NULL) {
		perror("Error opening file");
		return 1;
	}

	if(SAVE_IPS == 1){
		FILE *fs;
		fs = fopen(IP_LEFT, "r");
		if (fs == NULL) {
			//TODO: Copy IP_LIST To a new file IP_LEFT
		}
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
			
			//TODO: remove ip from ipsleft.txt
			//use a file lock and remove index 0! this SHOULD Work!!!!!

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

			sem_close(thread_sem);
			exit(0);

		}

	}

	fclose(fp);
	return 0;
}
