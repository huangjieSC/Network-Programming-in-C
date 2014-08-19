/*
** confclient.c -- a UDP conference client
*  Name: Jie Huang, Muhammad Nazmus Sakib
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "list.h"

#define MAXLINE 1024
#define max(a,b)    ((a) > (b) ? (a) : (b))


int main(int argc, char **argv)
{

	int maxfdp1;
	fd_set rset;
	FD_ZERO(&rset);

	CREC clientList,client;
	
	/* Variable and structure definitions. */
	int sd, rc;
	int port;
	struct sockaddr_in serveraddr, clientaddr, otherclientaddr;
	socklen_t serveraddrlen = sizeof(serveraddr);
	socklen_t clilen = sizeof(clientaddr);
	socklen_t otherclilen = sizeof(otherclientaddr);
	char server[255];
	char buffer[100];
	char *bufptr = buffer;
	int buflen = sizeof(buffer);
	struct hostent *hostp;
	memset(buffer, 0x00, sizeof(buffer));
	memcpy(buffer, "join", 5);
	char recvline[1024],sendline[1024];
	struct hostent *h;
	
	/* get a socket descriptor */
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("UDP Client - socket() error");
		/* Just exit lol! */
		exit(-1);
	}
	else
		printf("\nUDP conference Client is ready to connect....\n");

	if(argc > 1)
		strcpy(server, argv[1]);
	else
	{
		/*Use hostname or IP*/
		printf("UDP Client - Usage %s <Server hostname> <Server port>\n", argv[0]);
		exit(0);
	}
	 
	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	//serveraddr.sin_port = htons(SERVPORT);
	serveraddr.sin_port = htons(atoi(argv[2]));

	if((serveraddr.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE)
	{
		/* get server address */
		hostp = gethostbyname(argv[1]);

		if(hostp == (struct hostent *)NULL)
		{
			printf("HOST NOT FOUND --> ");
			exit(-1);
		}
		else
		{
			getsockname(sd,(struct sockaddr*)&clientaddr,&clilen);
			port=ntohs(clientaddr.sin_port);
			printf("Connected to UDP server %s on port %s.\n", argv[1], argv[2]);
		}
		memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
	}

	/* send request to server */
	rc = sendto(sd, bufptr, buflen, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(rc < 0)
	{
		perror("UDP Client - sendto() error");
		close(sd);
		exit(-1);
	}

	/* Read server reply. */

	rc = recvfrom(sd, bufptr, buflen, 0, (struct sockaddr *)&serveraddr, &serveraddrlen);
	 
	if(rc < 0)
	{
		perror("UDP Client - recvfrom() error");
		close(sd);
		exit(-1);
	}
	else
	{
		printf("\nUDP client received the list of clients from \n");
		printf("port %d, address %s\n", ntohs(serveraddr.sin_port), inet_ntoa(serveraddr.sin_addr));

		printf("The current client list excluding yourself is:\n%s\n",bufptr);

		if(strcmp(bufptr,"Client List Empty\n"))
		{
			clientList = parseFullList(bufptr, clientList);	
		}
	}

    /* receive and send messages from/to either server or other clients*/	
	for ( ; ; )  
	{
		FD_SET(fileno(stdin), &rset);
		FD_SET(sd, &rset);
		maxfdp1 = max(fileno(stdin), sd)  +  1;
		select(maxfdp1,  &rset,  NULL,  NULL,  NULL);
		if (FD_ISSET(sd,  &rset))  
		{ 
			bzero(recvline,1024);
			if(rc= recvfrom(sd, recvline,1024, 0, (struct sockaddr*)&otherclientaddr, &otherclilen) <= 0)
			{
		 		perror("recv error");
				exit(1);
		     	}
	            /*receive message from server, update the client list*/
		     	if(otherclientaddr.sin_addr.s_addr == serveraddr.sin_addr.s_addr && otherclientaddr.sin_port == serveraddr.sin_port)
		     	{	
				clientList = parseUpdate(recvline, clientList);				
			    }
			    /*receive message from other clients, print it out*/
		    	else
		    	{
				printf("\nMessage received from ");
	            if ((h = gethostbyaddr((char *) &otherclientaddr.sin_addr, sizeof otherclientaddr.sin_addr, AF_INET)) == NULL)
	  	        	perror("gethostbyaddr");
			    printf("host %s at port %d:\n%s", h->h_name, ntohs(otherclientaddr.sin_port), recvline);
               }
			
		}
		if (FD_ISSET(fileno(stdin), &rset))
		{  

			bzero(sendline,1024);
			if (fgets(sendline, MAXLINE, stdin) == NULL)
				break;  
            /*To exit, send exit message to server*/        
			if(!strcmp(sendline,"exit\n"))
			{
				///send leave msg to the server...	
				if(sendto(sd, "leave", 6, 0, (struct sockaddr*)&serveraddr, serveraddrlen)==-1)
					perror("Error sending leave msg");
				break;
			}
			/*Otherwise send message to all other clients*/
			else
			{
				/// send msg to all clients... 
				sendToAll(sd,sendline,clientList);
			}
		}
	}

	exit(0);
	return 0;
}
