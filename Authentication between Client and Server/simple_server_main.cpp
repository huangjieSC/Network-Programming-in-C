/**  Assignment #1
  Name: Jie Huang
  ID: 4625
  Date: Feb.2, 2011
  
  Program discription: Design and implement a concurrent server that can accecpt requests from
  many clients simultaneously. The requests can be processed at the same time, i.e. the server
  creates a child process for each client request and can receive and send data from/to clients
  at the same time. A maxinum of a success of three failures allowed. Once successfully verifying 
  the client or the maxinum number of failure has reached, close the child process and server uses
  signal() to prevent zombie child processes. Server is always on and ready to accept new connections
  until it is terminated by the user.
  */

#include "ServerSocket.h"
#include "SocketException.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <signal.h>

using namespace std;

void SigCatcher(int); // used in signal() to terminate dead child process
bool fexist(char *filename); // check whether the input file exists or not
void closeFile(ifstream &in); //close file after authentication finished
string checkPassword(ifstream &infile,string inbuf,string data1, string data2,  string output, bool &exist, bool &auth); // check whether the user-password pair is correct
void dostuff(ServerSocket &new_sock, char *filename, string output, string authmsg); // Wrapped function, used in main(), do all the passwork check and data sending/receiving work

int main ( int argc, char **argv)
{  int pid;
   int accept_sd; 
   
   string authmsg;
   string output;
   
    /* argument usage */
    if(argc!=3){
    cout<<"usage: a.out <Port Number> <Password File Name>"<<endl;
    return -1;}
    /* Notifying message*/
    cout << "running....\n";
    
    /* Check whether input file exists*/
     if( fexist(argv[2]))
                 cout << "Password file opened."<< endl;
                else{
                 cout << "File does not exist! Please try again." << endl;
                 return -1;
                 }
   
       /* Create a socket object, bind it to the port, and start listening*/
       ServerSocket server (atoi(argv[1]));
       cout << "Server is ready!" <<endl; 
       
      /* Terminate any zombie child*/
       signal(SIGCHLD,SigCatcher); 

      /* Accept clients' requests and create a child process to process each request, when finished, close the socket*/
       while(true)
     {  
        cout << "Waiting on accept()" << endl;
	    ServerSocket new_sock;
	    server.accept ( new_sock );
	    accept_sd = server.getaccept_sd(); // similar to connectfd in the sample code in class
	    cout << "Accept() is completed successfully! Waiting for requests from clients." << endl;
        
        pid = fork(); 
        if(pid < 0)
          cout << "Error on fork" << endl;
                   
        /* if in child, close parent listen socket, and start process client request*/
        if(pid == 0)
        {       cout << "In the child now." << endl;          
                server.~ServerSocket(); 
                dostuff(new_sock, argv[2], output, authmsg); // start the actual username/password check
                new_sock.~ServerSocket(); 
                exit(0);
        }           
        else
              new_sock.~ServerSocket();  // if in parent, close the child socket 
      } // end of while loop     
  return 0;
} 

void SigCatcher(int signo)
{   pid_t    pid;
    int      stat;

     while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
         printf("child %d terminated\n", pid);
     return;
}

/* Precondition: Give a file name as the input
   Postcondition: Return the status about whether this file exists or not*/

bool fexist(char *filename){
   ifstream ifile(filename);
   if(ifile)
      return true;
   else
      return false;
}

/*Simply close the file*/
void closeFile(ifstream &in){
     in.close();
}

/*Precondition: Given the name for the password file to check, user log in data, and default output
                message and status of authentication
  Postcondition: Return the message showing the authentication result in detail*/
string checkPassword(ifstream &infile,string inbuf, string data1, string data2, string output, bool &exist, bool &auth){
       while(!infile.eof())
                  {
                     getline(infile,inbuf,'\t');
                     if(inbuf==data1)
                      {exist = true;
                       string usermsg ("User found.");
                       getline(infile,inbuf);
                       if(inbuf==data2)
                          {  string passmsg1 ("Password verified.");
                             auth = true;
                             output = usermsg + " " + passmsg1;
                             break;
                          }
                       else{
                              string passmsg2 ("But password incorrect. Please try again.");
                              output = usermsg + " " + passmsg2;
                           }
                      }
                      else
                         getline(infile, inbuf); 
                  }  // end of while-loop
    return output;
}

/*Precondition: Given a socket to connect to the specific client and process the request; 
                also given the name for the password data; and default authentication messages
  Postcondition: Send the authentication result to the client*/
void dostuff(ServerSocket &new_sock, char *filename, string output, string authmsg)
{           int count = 0; //Counting number of failures
           
            while ( count<3 )
	       	  {    std::string data1;
                   std::string data2;

		           new_sock >> data1;  // Receive user name
                   new_sock >> data2;  // Receive password
                   cout<<"Now start authenticating."<<endl;
                   
                  ifstream infile;
                  infile.open(filename, ifstream::in);
                  string inbuf;
                  bool user_exist = false;                  
                  bool auth_success = false;          
                      
                  output = checkPassword(infile,inbuf,data1, data2, output, user_exist,auth_success);
                  closeFile(infile);
                  cout<<"Now finish authentication"<<endl;
                  
                  if(!user_exist) 
                     output = "User name not exist. Please try again.";
                  
                  /*Check if 3 failures happens, if yes, then notify client and close connection*/
                  if(!auth_success)
                    {  count++; 
                       if(count>=3){
                         authmsg = "exit";
                         cout << "3 failures, exit." << endl;}
                       else          
                       authmsg = output;
                    }
                  else {
                       authmsg = "Success";
                       new_sock << authmsg;
                      break;} 
                       
                  new_sock << authmsg;
		        } 
}
