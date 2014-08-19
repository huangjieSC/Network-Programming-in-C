/*
** peer.c -- Establishing a P2P Network
** Authors:
** Jie Huang, Muhammad Nazmus Sakib
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
#include <fcntl.h>
#include <errno.h>

int connect_nonb(int sockfd, struct sockaddr *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	error = 0;
	if((n = connect(sockfd, saptr, salen)) < 0)
		if (errno != EINPROGRESS)
			return (-1);
	/* Do whatever we want while the connect is taking place. */
	if (n == 0)
		goto done;               /* connect completed immediately */
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;
	if((n = select(sockfd + 1, &rset, &wset, NULL,nsec ? &tval : NULL)) == 0) 
	{
		close(sockfd);          /* timeout */
		errno = ETIMEDOUT;
		return (-1);
	}
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) 
	{
 		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        		return (-1);     /* Solaris pending error */
 	} else
   		perror("select error: sockfd not set");
	done:
		fcntl(sockfd, F_SETFL, flags);  /* restore file status flags */
	if (error) 
	{
    		close(sockfd);           /* just in case */
      		errno = error;
     		return (-1);
	}
	return (0);
}


void main(int argc, char *argv[])
{
	FILE *fp;
	struct sockaddr_in sin,servaddr;
	struct hostent *h;
	struct in_addr **addr_list;
 	struct sockaddr_in cli,cliaddr;
 	socklen_t len,clilen;
 	
 	fd_set master;
	fd_set read_fds;
	
	int listenfd, newfd, connfd, peerfd;
	int addrlen;  
	int port;
	int fdmax;
	int i,j;
	int msgCounter = 0;
	int nonce = 1;
	int counter=0,maxpeers;
	int nbytes,result;

	char msgCenter[1024][100];
	char buf[1024],buff[1024];
	char strName[1024],strPort[1024];
	char sendline[1024];
	char localhostname[100];

	if (argc != 4)
    {
	    printf("usage : peer <port> <maxpeers> <peersfile>");
	    exit(1);
	}
	
	maxpeers = atoi(argv[2]);
	addrlen=sizeof(sin);   
	memset(&sin, 0, sizeof(sin)); 
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);  
	
	sin.sin_family = AF_INET;  
	sin.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
	sin.sin_port=htons(atoi(argv[1]));

	bind(listenfd, (struct sockaddr *) &sin, sizeof(sin));  
	h = gethostbyaddr((char *) &sin.sin_addr.s_addr, sizeof(sin.sin_addr.s_addr), AF_INET);
	
	if(h!=NULL)
		printf("Using %s, listening at %s\n", h->h_name, argv[1]); 
	else
		perror("h is null");
	if (listen(listenfd, 1024) == -1) 
	{
		perror("listen");
		exit(1);
	}
	strcpy(localhostname,h->h_name);

    FD_SET(listenfd, &master);
	FD_SET(fileno(stdin), &master);
	fdmax = listenfd;

	fp = fopen(argv[3],"r");  	///try non blocking connect with all the peers
	while(fgets(strName,1024,fp)!=NULL && counter < maxpeers)
	{   
		fgets(strPort,1024,fp);
		peerfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(atoi(strPort));
		h = gethostbyname(strName);

		if(h!=NULL)
		{
			addr_list = (struct in_addr **)h->h_addr_list;
			memcpy(&servaddr.sin_addr, addr_list[0], sizeof(struct in_addr));

		}
		else
			;

		result = connect_nonb(peerfd, (struct sockaddr *) &servaddr, sizeof(servaddr),0);
		clilen = sizeof(cli);
 	    getsockname(peerfd,(struct sockaddr*)&cli,&clilen);
		port=ntohs(cli.sin_port);

		if(result < 0)
		{
			//perror("couldnt connect\n");
		}
		else
		{
			printf("connected to peer on %s at %s thru %d\n",strPort,strName,port);
			FD_SET(peerfd, &master); // add to master set
            if (peerfd > fdmax) 
			{  fdmax = peerfd; }  // keep track of the max
		}

		counter++; // keep track of the allowed number of connections on the list
	} // end of while
	
	for(;;) 
	{
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
		{ perror("select"); exit(1);}
     
        for(i = 0; i <= fdmax; i++)   // run through the existing connections looking for data to read
		{
            if (FD_ISSET(i, &read_fds)) // $$ we got one discriptor set
		    { 	 
              if (i == listenfd) // <1> handle new connections
			  {
			    len = sizeof(cliaddr);
			    newfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len); //accept connection from client
			                    
			    if (newfd == -1)   { perror("accept");} 
                else
			     {
       	             FD_SET(newfd, &master); // add to master set
                     if (newfd > fdmax) {  fdmax = newfd; }  // keep track of the max
				      
                     printf("New connection from %s, socket %d\n",
			            inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),newfd);	
                 } // end of else 
               } // end of <1>       
               
              else if (i == fileno(stdin)) // <2> input is readable, handle std input
		      {
				char sendwithnonce[1024];
				bzero(sendline,1024);

				if (fgets(sendline, 1024, stdin) == NULL)   /* all done */
				{  perror("done\n"); return; }
				
				sprintf(sendwithnonce,"%8d%s:%s->%s",nonce++,localhostname,argv[1],sendline);
				for(j = 3; j <= fdmax; j++) //send input to all the connected hosts
				{   if (FD_ISSET(j, &master)) 
				    {  
                      if (j != listenfd && j != i) // except the listener and ourselves
					  {               
		                if (send(j, sendwithnonce, strlen(sendwithnonce), 0) == -1) 
				 	     perror("send");
                      }
                    }
              	}//end of for - send to all
              } // end of <2> - input is readable       
             
              else // <3> handle data from a client
		      {
                 bzero(buf,1024); 
                 if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)  // got error or connection closed by client
		         {   
                    if (nbytes == 0) // connection closed
			        {  printf("selectserver: socket %d hung up\n", i);}
                    else
			        {  perror("recv"); }
                    
                    close(i); 
                    FD_CLR(i, &master); // remove from master set
                 } // end of if - got error or connection closed 
                 
                 else  // we got some data from a client and send to all others
                 {   
			         int k,flag=0;
                     char msg2show[1024];
                	
			         for(k=0;k<msgCounter;k++)  ///check to see if the msg has been received before
			         {
                       if(!strcmp(buf,msgCenter[k]))
				       {
                         flag = 1;	
					     break;
	                   }
                     }

			         if(flag)
                       continue;	
                       
                     strcpy(msgCenter[msgCounter++],buf);		
		             strcpy(msg2show,buf+8);
		             printf("%s",msg2show);
                     for(j = 3; j <= fdmax; j++) //send input to all the connected hosts
		             {   
                         if (FD_ISSET(j, &master))
                         {  
                           if (j != listenfd && j != i) // except the listener and the source client
                           {        
                             if (send(j, buf, strlen(buf), 0) == -1) 
                               perror("send");
                           }
                         }
                     }//end of for - send to all
                 } // end of else - got data from a client and send it to all others
                
              } // end of <3> handle data from client
            } // end of $$
        } // end of traversing through file descriptors
    } // END for(;;)
	
}

