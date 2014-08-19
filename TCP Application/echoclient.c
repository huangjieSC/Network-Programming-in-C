/*
** echoclient.c -- a TCP preforked server with file locking around accept
** Authors:
** Muhammad Nazmus Sakib
** Jie Huang
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
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "util.h"


int main(int argc, char *argv[])
{
	int     sockfd;
	struct 	sockaddr_in servaddr;
	struct 	sockaddr_in cli;
	char 	sendline[1024], recvline[1024];
	int 	success=0,n;
	int 	result;
	int nbytes;
	int port;

	int addrlen,clilen;

	int res;
	struct hostent *h;
	struct in_addr **addr_list;
	int pid;

	if(argc != 3)
	    {
	     perror("usage: echoclient <servhost> <servport>");
	     exit(1);
	    }


        sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	h = gethostbyname(argv[1]);

	if(h!=NULL)
	{
		addr_list = (struct in_addr **)h->h_addr_list;
		memcpy(&servaddr.sin_addr, addr_list[0], sizeof(struct in_addr));

	}
	else
		perror("h null");


	//connect to server...
	result = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	clilen = sizeof(cli);
    	getsockname(sockfd,(struct sockaddr*)&cli,&clilen);
	port=ntohs(cli.sin_port);

	if(result==-1)
	{
		perror("couldnt connect\n");
		return;
	}

	printf("connected to echoserver on %s at %s thru %d\n",argv[1],argv[2],port);


        if((pid = fork()) == 0)
	{
		while(1)
		{
			bzero(sendline,1024);

			if (fgets(sendline, 1024, stdin) == NULL)
			{
				perror("done\n");
						return;          /* all done */
			}

			send(sockfd, sendline, strlen(sendline), 0);
		}
		

	}
	else
	{
		while(1)
		{
			bzero(recvline,1024);
			 if ((nbytes = recv(sockfd, recvline, sizeof recvline, 0)) <= 0)
			     {
		         perror("server terminated prematurely");
					 exit(1);
			     }

			 fputs(recvline, stdout);
		}
	
	}

	return 0;
}

