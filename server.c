#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h> 

#define TRUE 			1
#define FALSE			0
#define BUFFER_SIZE 	1024
#define	LISTEN_PORT		60000
#define SHELL "/bin/sh"

void connection_display();

int main(int argc, char *argv[])
{
	
	puts("hello world");
	signal(SIGCHLD,SIG_IGN);

	int opt = TRUE;
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;

	char buffer[BUFFER_SIZE + 1]; // read the characters from socket

	fd_set readfds;

	char *welcome_message = "Welcome to C Chat \r\n";

	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	if ( (master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		fprintf(stderr, "Master socket failed\n");
		exit(0);
	}

	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
    	fprintf(stderr, "Failes to facilitate multiple connections\n" );
        exit(EXIT_FAILURE);
    }

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( LISTEN_PORT );

	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		fprintf(stderr, "Bind failed\n" );
		exit(1);
	}

	// connection_display();
	
	printf("********************************************************");
	printf("\n\tServer listening on port %d\n", LISTEN_PORT);
	printf("********************************************************");

	// try to specify a maxium of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		fprintf(stderr, "Socket listen error\n" );
		exit(1);
	}

	// signal(SIGTERM, close_socket);

	addrlen = sizeof(address);

	puts("\nWaiting for connections...\n");

	while (TRUE)
	{
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);

		max_sd = master_socket;

		for (i=0; i<max_clients; i++)
		{
			sd = client_socket[i];

			if (sd > 0)
				FD_SET(sd, &readfds);

			if (sd > max_sd)
				max_sd = sd;

		}

		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			fprintf(stderr, "Select function error\n" );
			exit(1);
		}

		// handles incomming connectiosn from the master socket
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(1);
			}

			//inform user of socket number - used in send and receive commands
		    printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

		    //send new connection greeting message
		    if( send(new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) )
		    {
		        perror("send");
		    }
		      
		    puts("Welcome message sent successfully");

		    for (i=0; i< max_clients; i++)
		    {
		    	if (client_socket[i] == 0)
		    	{
		    		client_socket[i] = new_socket;
		    		printf("Adding to list of sockets as %d\n", i);
		    		break;
		    	}
		    }

		}

		for (i=0; i<max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
				{
					getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
				}
				else
				{
					buffer[valread] = '\0';
					send(sd, buffer, strlen(buffer), 0);
				}
			}
		}
	}

	return 0;
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