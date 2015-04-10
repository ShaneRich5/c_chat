#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define BUFFER_SIZE 256

void main(int argc, char *argv[])
{
	int sockfd, portno, n;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[BUFFER_SIZE];

	portno = atoi("60000");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		fprintf(stderr, "Error opening socket\n");
		exit(0);
	}

	server = gethostbyname("localhost");

	if (server == NULL)
	{
		fprintf(stderr, "Error, no host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
    	fprintf(stderr, "Error connecting\n");
    	exit(0);
    }

    system("clear");

    printf("*************************************************\n");
    printf("*\t\tWelcome to C Chat\t\t*\n");
    printf("*************************************************\n");

    while(1)
    {
    	puts("1 - Broadcast");
    	puts("2 - Peer to Peer")


		printf("\n> ");
    
	    bzero(buffer, BUFFER_SIZE);
	    fgets(buffer, (BUFFER_SIZE - 1), stdin);

	    if (0 == strncmp(buffer, "Finish", 6))
	    {
	    	printf("\nAlert! Communication with server terminated...\n");
	    	break;
	    }

	    n = write(sockfd, buffer, strlen(buffer));

	    if (n < 0)
	    {
	    	fprintf(stderr, "Error writing to socket\n");
	    	exit(0);
	    }

	    bzero(buffer, BUFFER_SIZE);

	    n = read(sockfd, buffer, (BUFFER_SIZE - 1));

	    if (n < 0)
	    {
	    	fprintf(stderr, "Error reading from socket");
	    	exit(0);
	    }
	    printf("%s\n", buffer);
    }
}