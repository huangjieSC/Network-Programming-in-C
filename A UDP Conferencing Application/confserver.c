/*
** confserver.c -- a UDP conference server
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


/* Run the server without argument */
int main(int argc, char *argv[])
{
	/* Variable and structure definitions. */
	int sd, rc;
	int port;
	struct sockaddr_in serveraddr, clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	socklen_t serveraddrlen = sizeof(serveraddr);
	char buffer[100];
	char *bufptr = buffer;
	int buflen = sizeof(buffer);
	char hostname[1024];
	hostname[1023]='\0';
	char *bcMsg,*listMsg;
	struct hostent *h;

	CREC clientList=NULL;
	CREC client;
 
	/* get a socket descriptor */
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("UDP server - socket() error");
		exit(-1);
	}
	else
		printf("Welcome to UDP conference server....\n");
	 
	/* bind to address */
	memset(&serveraddr, 0x00, serveraddrlen);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(0);


	if((rc = bind(sd, (struct sockaddr *)&serveraddr, serveraddrlen)) < 0)
	{
		perror("UDP server - bind() error");
		close(sd);
		/* If something wrong with socket(), just exit */
		exit(-1);
	}
	else
	{
		getsockname(sd,(struct sockaddr*)&serveraddr,&serveraddrlen);
		port=ntohs(serveraddr.sin_port); //getting the port number
		gethostname(hostname,1023); // get local host name
		h = gethostbyname(hostname); // perform a lookup with gethostbyname()

		printf("UDP conference server is ready at hostname %s and port %d\n", h->h_name, port);
	}

	 
	while(1)
	{
		 printf("\n\nUDP Server is ready to get new connections from client...\n");

		/* Wait on client requests. */
		rc = recvfrom(sd, bufptr, buflen, 0, (struct sockaddr *)&clientaddr, &clientaddrlen);
		if(rc < 0)
		{
			perror("UDP Server - recvfrom() error");
			close(sd);
			exit(-1);
		}

		if ((h = gethostbyaddr((char *) &clientaddr.sin_addr, sizeof clientaddr.sin_addr, AF_INET)) == NULL)
	  		perror("gethostbyaddr");

		if(!strcmp(bufptr,"join"))
		{              
			printf("UDP Server received a join in message from host %s at port %d, address is %s.\n", h->h_name, ntohs(clientaddr.sin_port), inet_ntoa(clientaddr.sin_addr));

			client = (CREC)malloc(sizeof(CLIENT_REC));						
			client->cliaddr = clientaddr;
			
			listMsg = getCurrentClientList(clientList);
			if(sendListToClient(sd,client,listMsg)==-1)
				perror("Error sending list");		
                	
			///send update to all other recipient
			bcMsg = getNewClientStatus(client,1);
			if(sendUpdateToClients(sd,clientList,bcMsg)==-1)
				perror("Error sending update");			
			
			///insert
			clientList = insertIntoClientList(clientList,client);	 
		}
		else if(!strcmp(bufptr,"leave"))
		{
			printf("UDP Server received a leave out message from port %d and address %s.\n", ntohs(clientaddr.sin_port), inet_ntoa(clientaddr.sin_addr));

			client = (CREC)malloc(sizeof(CLIENT_REC));
			client->cliaddr = clientaddr;
			
	
			///send update to all other recipient
			bcMsg = getNewClientStatus(client,0);
			if(sendUpdateToClients(sd,clientList,bcMsg)==-1)
				perror("Error sending update");			
			
			///remove
			clientList = removeFromClientListByAddress(clientList,client);
			
		}		

	} // END OF WHILE(1)
	 
	/* When the data has been sent, close() the */
	/* socket descriptor. */
	/********************************************/
	/* close() the socket descriptor. */
	close(sd);
	exit(0);
}


