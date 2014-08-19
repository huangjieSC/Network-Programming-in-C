/* 
*  Name: Jie Huang, Muhammad Nazmus Sakib
*/

#ifndef LIST_H
#define LIST_H

typedef struct client_rec
{
	struct sockaddr_in cliaddr;
	struct client_rec *next;
} CLIENT_REC,*CREC;


CREC insertIntoClientList(CREC clientList,CREC newUDPClient);
CREC removeFromClientListByAddress(CREC clientList, CREC client);
int sendUpdateToClients(int sockfd, CREC client,char *msg);
char* getCurrentClientList(CREC clientList);
char* getNewClientStatus(CREC client, int action);
int sendListToClient(int sockfd, CREC client,char *msg);
void sendToAll(int sockfd,char *msg,CREC clientList);
CREC parseFullList(char *list, CREC clientList);
CREC parseUpdate(char *update, CREC clientList);

#endif
