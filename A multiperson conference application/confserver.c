/*
** confserver.c -- a multiperson conference server
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


int main(int argc, char *argv[])
{

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number


	int newfd;        // newly accept()ed socket descriptor
    
    char hostname[1024];
    hostname[1023]='\0';
	char buf[1024],buff[1024];    // buffer for client data
	char clientnames[100][1024];  // client list
	char climsg[1024];			  // msg for all client
	int nbytes;

	int i, j, rv;

	struct   sockaddr_in sin;
	struct hostent *h;
	int addrlen;
	int port;

	int     listenfd, connfd;
 	socklen_t len;
 	struct sockaddr_in servaddr, cliaddr;
    struct in_addr myhost;



	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);


	//creating the socket to listen to...
	printf("Welcome to conference server....\n");

	addrlen=sizeof(sin);
	memset(&sin, 0, sizeof(sin));
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr=htonl(INADDR_ANY);
	sin.sin_port=htons(0); // so that the kernel reserves a unique port for us

	bind(listenfd, (struct sockaddr *) &sin, sizeof(sin));
	getsockname(listenfd,(struct sockaddr*)&sin,&addrlen);
	port=ntohs(sin.sin_port); //getting the port number
	myhost.s_addr = htonl(INADDR_LOOPBACK);  /// for getting the local host name
	//h = gethostbyaddr((char *) &myhost.s_addr, sizeof(myhost.s_addr), AF_INET);
    
    gethostname(hostname,1023);
   // printf("Hostname: %s\n",hostname);
    h = gethostbyname(hostname);
    int k;
	if(h!=NULL)
        printf("started server on %s at %d\n", h->h_name, port);
   	else
		perror("h is null");


	// listen
	if (listen(listenfd, 1024) == -1)
	{
		perror("listen");
		exit(1);
	}



	// add the listener to the master set
    FD_SET(listenfd, &master);


	// keep track of the biggest file descriptor
	fdmax = listenfd; // so far, it's this one

	// main loop
	for(;;)
	{
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
		    perror("select");
		    exit(1);
		}

	    // run through the existing connections looking for data to read
       	for(i = 0; i <= fdmax; i++)
		{

	        if (FD_ISSET(i, &read_fds))
		    {
				// we got one
				if (i == listenfd)
				{
	                 // handle new connections
			    	len = sizeof(cliaddr);
			    	newfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len); //accept connection from client


			    	if (newfd == -1)
			     	{
                        perror("accept");
                   	}

			     	else
			     	{
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax)
						{
				   			// keep track of the max
 	                       	fdmax = newfd;
                        }

						printf("server: connect from %s, socket# %d\n",
			            inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
			            newfd);

						if (getpeername(newfd, (struct sockaddr *) &cliaddr, &len) < 0)
 					       perror("getpeername");
						else
						{
					        if ((h = gethostbyaddr((char *) &cliaddr.sin_addr,
  		                                sizeof cliaddr.sin_addr,
  			                                AF_INET)) == NULL)
  					          perror("gethostbyaddr");
					        else
					        {
					        	printf("remote host is '%s'\n", h->h_name);
					        	strcpy(clientnames[newfd],h->h_name);
							}
						}

                    }
             	}

				else
				{
                   	bzero(buf,1024); // NEW...........
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
				    {
                        // got error or connection closed by client
                        if (nbytes == 0)
						{
                            // connection closed
                            printf("server: %s hung up, socket %d\n",clientnames[i], i);
                        }
						else
						{
                            perror("recv");
                        }
                        close(i); //
                        FD_CLR(i, &master); // remove from master set
                    }
				    else
				    {
                        // we got some data from a client
                        printf("%s>>>%s",clientnames[i],buf); //NEW..........
						sprintf(climsg,"%s>>>%s",clientnames[i],buf);
                        for(j = 0; j <= fdmax; j++)
						{
                            // send to everyone!
                            if (FD_ISSET(j, &master))
			    			{
                                // except the listener and ourselves
                                 if (j != listenfd && j != i)
								 {
                                    //if (send(j, buf, nbytes, 0) == -1)
                                    if (send(j, climsg, strlen(climsg), 0) == -1)
				 					{
                                        perror("send");
                                    }

                                 }

                            }

                        }//end of for

                    } // end of else

                } // END handle data from client

            } // END got new incoming connection

        } // END looping through file descriptors

    } // END for(;;)

    return 0;
}

