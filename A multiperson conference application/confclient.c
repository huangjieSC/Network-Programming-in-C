/*
** confclient.c -- a conference client
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

#define MAXLINE 1024
#define max(a,b)    ((a) > (b) ? (a) : (b))


void main(int argc, char **argv)
{
	int     sockfd;
	struct 	sockaddr_in servaddr;
	struct 	sockaddr_in cli;
	char 	sendline[1024], recvline[1024];
	int 	success=0,n;
	int 	result;
	int nbytes;
	int port;

	int maxfdp1;
	int addrlen,clilen;
	fd_set rset;
	FD_ZERO(&rset);
	int res;
	struct hostent *h;
	struct in_addr **addr_list;



	//check for required program arguments
    if (argc != 3)
    {
    	    printf("usage : confclient <servhost> <servport>");
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

	printf("connected to server on %s at %s thru %d\n",argv[1],argv[2],port);

	for ( ; ; )
	{
		 FD_SET(fileno(stdin), &rset);
		 FD_SET(sockfd, &rset);
		 maxfdp1 = max(fileno(stdin), sockfd)  +  1;
		 select(maxfdp1,  &rset,  NULL,  NULL,  NULL);

	     if (FD_ISSET(sockfd,  &rset))
		 {
			 /* socket is readable */
		     bzero(recvline,1024);
	         if ((nbytes = recv(sockfd, recvline, sizeof recvline, 0)) <= 0)
		     {
                 perror("server terminated prematurely");
				 exit(1);
		     }

	         fputs(recvline, stdout);
         }

         if (FD_ISSET(fileno(stdin), &rset))
		 {
		    /*  input is readable */
		    bzero(sendline,1024);

			if (fgets(sendline, MAXLINE, stdin) == NULL)
			{
				perror("done\n");
						return;          /* all done */
			}

			send(sockfd, sendline, strlen(sendline), 0);

         }

    }

	exit(0);
}
