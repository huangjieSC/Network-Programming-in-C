/**  Assignment #1
  Name: Jie Huang
  ID: 4625
  Date: Feb.2, 2011
  
  Program discription: Design and implement a client that can connect to the server
  and send its username/password information to the server to get authenticated. If
  authentication failed, client prompts the user again and sends it to the server.
  A maxinum of three failures allowed. Client receives the result from server and 
  displays it. Once successfully getting verified or the maxinum number of failures reached,
  client closes the connection and exits. 
  */


#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using namespace std;

int main ( int argc, char **argv)
{  char username[256];
   string defaultresult ("Success"); 
   string defaultsignal ("exit");
   
   /*Argument usage*/
   if(argc!=3){
     cout<<"usage: a.out <IPaddress> <Port number>"<<endl;
     return -1;}

  try
    {     
         std::string authmsg;
         string IPaddress(argv[1]);
        
        /*Create a client socket object using the IP address and port number provided by user*/
         ClientSocket client_socket(IPaddress,atoi(argv[2]));
       
         cout << "Connecting to the server...."<<endl;         
         
        /*Use a do-while loop to keep prompting for user name and password input until being
          verified or 3 failures reached, information received from server*/
      do {     
             cout<<"Please enter your user name:"<<endl; 
             cin.getline(username, 256);
   
             char *password = getpass("Plese enter your password: ");
             /*masking the password and replace it with stars*/
             int pass_len;
             pass_len = strlen(password);
             int i;
             for(i=0;i<pass_len;i++)
               cout<<"*";
             cout<<endl;  
        /*Send information to server and receive authentication result*/
        try
         {
             client_socket<<username;
             client_socket<<password;
                       
	         client_socket >> authmsg;
         }
        catch ( SocketException& ) {}
        
             std::cout << authmsg << "\n"; // Print out the result
      } while(authmsg.compare(defaultresult)!=0 && authmsg.compare(defaultsignal) !=0); //end of do-while
        
        if(authmsg.compare(defaultresult) ==0)
          cout << "You are logged in now.\n" << endl;
        
        if(authmsg.compare(defaultsignal) ==0)
          cout << "Maximum number of trial reached.\n" << endl;
      
       client_socket.~ClientSocket();  // Close socket connection 
    }// end of try
  catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\n";
    }
   exit(0);
  return 0;
}
