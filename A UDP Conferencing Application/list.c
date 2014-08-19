/* list.c file to create and manage client list
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

CREC insertIntoClientList(CREC clientList,CREC newUDPClient)
{
	CREC head = clientList;


	if(clientList == NULL)
	{
		clientList = (CREC)malloc(sizeof(CLIENT_REC));		
		clientList->cliaddr = newUDPClient->cliaddr;
		clientList->next = NULL;
		return clientList;
	}
	else
	{
		while(clientList->next!=NULL)
		{
			clientList = clientList->next;
		}

		clientList->next = (CREC)malloc(sizeof(CLIENT_REC));
		clientList->next->cliaddr = newUDPClient->cliaddr;
		clientList->next->next = NULL;
	}
		
	return head;
}



CREC removeFromClientListByAddress(CREC clientList, CREC client)
{
	CREC prev = clientList, head = clientList;

	while(clientList!=NULL)
	{
		if(clientList->cliaddr.sin_addr.s_addr==client->cliaddr.sin_addr.s_addr && clientList->cliaddr.sin_port==client->cliaddr.sin_port )
		{
			prev->next = clientList->next;			
			break;
		}
		prev = clientList;
		clientList = clientList->next;	
	}	

	return head;
}


char* getCurrentClientList(CREC clientList)
{  
	char *str;
	int count=0;
	
	
	CREC head = clientList;


	while(clientList!=NULL)
	{
		count++;		
		clientList = clientList->next;
	}


	if(count == 0)
	{
		str = (char*) malloc(50);
		strcpy(str,"Client List Empty\n");
		return str;
	}

	str = (char*) malloc(count*32);
	
	sprintf(str,"%s,%d,%u\n",inet_ntoa(head->cliaddr.sin_addr),ntohs(head->cliaddr.sin_port),1);
	clientList = head->next;
	
	while(clientList!=NULL)
	{
		sprintf(str,"%s%s,%d,%u\n",str,inet_ntoa(clientList->cliaddr.sin_addr),ntohs(clientList->cliaddr.sin_port),1);
		clientList = clientList->next;		
	}
	
	printf("list: %s",str);

	return str;

}

char* getNewClientStatus(CREC client, int action)
{
	char *str;
	
	str = (char*) malloc(32);
	sprintf(str,"%s,%d,%d",inet_ntoa(client->cliaddr.sin_addr),ntohs(client->cliaddr.sin_port),action);
	printf("single client: %s",str);
	return str;
}

int sendListToClient(int sockfd, CREC client,char *msg)
{
	if(client!=NULL)
	{
		if(sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&client->cliaddr, sizeof(client->cliaddr)) == -1)
		{
			perror("error sending update msg");
			return -1;
		}
	}

	return 0;
}

int sendUpdateToClients(int sockfd, CREC clientList,char *msg)
{
	while(clientList!=NULL)
	{
		if(sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*) &clientList->cliaddr,sizeof(clientList->cliaddr)) == -1)
		{
			perror("error sending update msg");
			return -1;
		}
		clientList = clientList->next;
	}

	return 0;
}

void sendToAll(int sockfd,char *msg,CREC clientList)
{
	struct sockaddr_in destaddr;

	while(clientList!=NULL)
	{
		memset(&destaddr, 0x00, sizeof(struct sockaddr_in));
		destaddr.sin_family = AF_INET;
		destaddr.sin_addr = clientList->cliaddr.sin_addr;
		destaddr.sin_port = clientList->cliaddr.sin_port;
		if(sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&destaddr, sizeof(destaddr)) == -1)
		{
			perror("error sending msg");
		}
		clientList = clientList->next;
	}
}

CREC parseFullList(char *list, CREC clientList)
{
	char *token;
	CREC client;
	int action;
	
	token = strtok(list,",");

	
	while(token != NULL)
	{
		client = (CREC)malloc(sizeof(CLIENT_REC));
		client->cliaddr.sin_addr.s_addr = inet_addr(token);
		
		token = strtok(NULL,",");		
		client->cliaddr.sin_port = htons(atoi(token));
			
		token = strtok(NULL,"\n");
		action = atoi(token);
		
		if(action)
		{
			clientList = insertIntoClientList(clientList, client);
		}
		token = strtok(NULL,",");
	}
	
	return clientList;
		
}


CREC parseUpdate(char *update, CREC clientList)
{

	char *token;
	CREC client;
	int action;
	
	token = strtok(update,",");

	if(token == NULL)
		return NULL;
	
	client = (CREC)malloc(sizeof(CLIENT_REC));
	
	client->cliaddr.sin_addr.s_addr = inet_addr(token);
	
	token = strtok(NULL,",");		
	client->cliaddr.sin_port = htons(atoi(token));
		
	token = strtok(NULL,",");		
	action = atoi(token);
	
	if(action)
	{
		printf("New Client joined: %s\n",update);
		return insertIntoClientList(clientList, client);
	}
	else
	{
		printf("Client left: %s\n",update);
		return removeFromClientListByAddress(clientList, client);
	}
	
}
