#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

static int nchildren;
static pid_t *pids;
static struct flock lock_it, unlock_it;
static int lock_fd = -1;  /* fcntl() will fail if my_lock_init() not called */


void sig_int(int);
pid_t child_make(int, int, int);
void child_main(int, int, int);
void child_dostuff(int);
void my_lock_init(char *);
void my_lock_wait();
void my_lock_release();

