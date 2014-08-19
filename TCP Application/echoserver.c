/*
** echoserver.c -- a TCP preforked server with file locking around accept
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
    int listenfd, i;
    socklen_t addrlen;
    struct   sockaddr_in sin;
    
    if(argc != 3)
    {
     perror("usage: echoserver <port> <children>");
     exit(1);
    }
    
    nchildren = atoi(argv[2]);
    pids = calloc(nchildren, sizeof(pid_t));
    
    //creating the socket to listen to...
	printf("Welcome to TCP preforked server....\n");

	addrlen=sizeof(sin);
	memset(&sin, 0, sizeof(sin));
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr=htonl(INADDR_ANY);
	sin.sin_port=htons(atoi(argv[1]));
	
    bind(listenfd, (struct sockaddr *) &sin, sizeof(sin));

// listen
	if (listen(listenfd, 1024) == -1) 
	{
		perror("listen");
		exit(1);
	}

 
    addrlen = sizeof(sin);

     my_lock_init("/tmp/lock.XXXXXX"); /* one lock file for all children */

     for (i = 0; i < nchildren; i++)
         pids[i] = child_make(i, listenfd, addrlen); /* parent returns */

     signal(SIGINT, sig_int);

     for ( ; ; )
         pause();                /* everything done by children */
}











