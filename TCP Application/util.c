#include "util.h"

//#define MAXN    16384     /* max # bytes client can request */


void sig_int(int signo)
{
     int     i;
    // void    pr_cpu_time(void);

         /* terminate all children */
     for (i = 0; i < nchildren; i++)
         kill(pids[i], SIGTERM);
     while (wait(NULL) > 0)     /* wait for all children */
         ;
     if (errno != ECHILD)
         perror("wait error");

     //pr_cpu_time();
     exit(0);
}


pid_t child_make(int i, int listenfd, int addrlen)
{
    pid_t   pid;

    if ( (pid = fork()) > 0)
        return (pid);            /* in parent, return child's pid */
    
    // in child
     child_main(i, listenfd, addrlen);     /* never returns */
}


void child_main(int i, int listenfd, int addrlen)
{
    int     connfd;
    socklen_t clilen;
    struct sockaddr *cliaddr;

    cliaddr = (struct sockaddr *)malloc(addrlen);

    printf("child %ld starting\n", (long) getpid());
    for ( ; ; ) {
        clilen = addrlen;
        
        my_lock_wait();
        
        connfd = accept(listenfd, cliaddr, &clilen);

        my_lock_release();

        child_dostuff(connfd);      /* process the request */
        close(connfd);
    }
}

void my_lock_init(char *pathname)
{
    char     lock_file[1024];

        /* must copy caller's string, in case it's a constant */
    strncpy(lock_file, pathname, sizeof(lock_file));
    lock_fd = mkstemp(lock_file);

    unlink(lock_file);          /* but lock_fd remains open */

    lock_it.l_type = F_WRLCK;
    lock_it.l_whence = SEEK_SET;
    lock_it.l_start = 0;
    lock_it.l_len = 0;

    unlock_it.l_type = F_UNLCK;
    unlock_it.l_whence = SEEK_SET;
    unlock_it.l_start = 0;
    unlock_it.l_len = 0;
}


void child_dostuff(int sockfd)
{   char buf[1024];    // buffer for client data
    int ntowrite;
    int nread;
    int nbytes;
//    char    line[MAXLINE], result[MAXN];

    for ( ; ; ) 
    {
        /* connection closed by other end */  
        bzero(buf,1024);
        if ((nbytes = recv(sockfd, buf, sizeof buf, 0)) <= 0)
           return;  // got error or connection closed by client
            
        else
        { // we got some data from a client
           printf("%s",buf); 
		   if (send(sockfd, buf, strlen(buf), 0) == -1)
		      perror("send");
        }  
    }
}



/* include my_lock_wait */
void
my_lock_wait()
{
    int		rc;
    
    while ( (rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
		if (errno == EINTR)
			continue;
    	else
			perror("fcntl error for my_lock_wait");
	}
}

void
my_lock_release()
{
    if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0)
		perror("fcntl error for my_lock_release");
}
/* end my_lock_wait */

