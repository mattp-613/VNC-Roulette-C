#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
	char ip[17];
	FILE *fp;
	fp = fopen("iplist.txt", "r");
	if (fp == NULL) {
		perror("Error opening file");
		return 1;
	}

	while(fgets(ip, 17, fp)) {
		printf("Scanning %s",ip);
		ip[strcspn(ip, "\n")] = 0; // remove newline character

		char command[69];
		
		sprintf(command, "timeout 10 vncsnapshot -rect 0x0-800-600 %s:0 snapshot%s.jpg", ip, ip);

		if(fork() == 0) {
			int timeout = system(command);
			if(timeout != 0) {
    			printf("IP %s timed out.\n",ip);
			}
			exit(0);
		}

		/*
		else { //TODO: initially just waiting for fork to complete (set limit to forks for number of thread)
			wait(NULL);
		}*/

	}

	fclose(fp);
	return 0;
}
