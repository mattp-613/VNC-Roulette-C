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
		ip[strcspn(ip, "\n")] = 0; // remove newline character

		char command[68];
		sprintf(command, "xwd -root -display %s:0 | convert xwd:- screenshot%s.jpg", ip, ip);

		pid_t pid = fork();
		if(pid == 0) {
			system(command);
			exit(0);
		}
		else {
			wait(NULL);
		}
	}

	fclose(fp);
	return 0;
}
