#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256
#define SHELL "/bin/sh"

void connection_display();

void main(int argc, char *argv[])
{
	signal(SIGCHLD,SIG_IGN);

	char *commands[4];

	commands[0] = "sh";
	commands[1] = "-c";
	commands[3] = NULL;

	int sockfd, newsockfd; // file descriptors
	int portno; // port number on which the server accepts connections
	int clilen; // stores the clients address size
	int n; // the return value for the read() and write() calls

	char buffer[BUFFER_SIZE]; // read the characters from socket

	struct sockaddr_in serv_addr, cli_addr; 

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0)
	{
		fprintf(stderr, "Error, failed to open socket\n");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); // set all values in buffer to zero

	portno = atoi("60000"); // listen on port 60000

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno); // converts port number to network byte order
	serv_addr.sin_addr.s_addr = INADDR_ANY; //host ip addr

	if (bind(sockfd, (struct  sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "Error on binding\n");
		exit(1);
	}

	connection_display();
	
	printf("\n\n********************************************************");
	printf("\n\t\tServer listening on port 60000**\n");
	printf("********************************************************");

	printf("\nWaiting on commands...\n");

	listen(sockfd, 5); // call assumes socket i valid from previous check

	clilen = sizeof(cli_addr);

	while (1) // supports multiple connections using multithreading
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0)
		{
			fprintf(stderr, "Error on accept\n");
			exit(1);
		}

		int pid = fork();

		if (pid < 0) {
			fprintf(stderr, "Error forking process\n");
			exit(1);
		} else if (pid == 0) {
			while(1)
			{
				bzero(buffer, BUFFER_SIZE);
				
				n = read(newsockfd, buffer, 255);

				if (n < 0)
				{
					fprintf(stderr, "Error reading from socket\n");
					exit(1);
				}

				printf("received command: %s", buffer);

				if (strncmp(buffer, "Finish", 6) == 0)
				{
					write(newsockfd, "\nServer side connection ended...", 25);
					break;
				}

				system(buffer);

				n = write(newsockfd, "Command executed", 18);

				if (n < 0)
				{
					fprintf(stderr, "Error writing to socket");
					exit(1);
				}

			} 
			close(newsockfd);
			exit(0);	
		} else {
			close(newsockfd);
		}
	}

	close(sockfd);
}

void connection_display()
{
	int i, j;

	printf("Connecting in...");

	for (i = 3; i >= 0; i--)
	{
		for (j = 0; j <= 1000000; j++)
		{

		}
		printf("%d...", i);
	}
	system("clear");
}