/**  Assignment #1
  Name: Jie Huang
  ID: 4625
  Date: Feb.2, 2011
  
  Program discription: Design and implement an iterative server that can accecpt requests from 
  the client one by one, only one request can be processed at each time. Receive user login 
  information and send the authentication result back to the client. A maxinum of a success of 
  three failures allowed. Once successfully verifying the client or the maxinum number of failure
  has reached, close the connection and ready to accept the request from the next client. Server 
  is always on and ready to accept new connections until it is terminated by the user.
  */

#include "ServerSocket.h"
#include "SocketException.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

bool fexist(char *filename); // check whether the input file exists or not
void closeFile(ifstream &in); //close file after authentication finished
string checkPassword(ifstream &infile,string inbuf,string data1, string data2,  string output, bool &exist, bool &auth); // check whether the user-password pair is correct
void dostuff(ServerSocket &new_sock, char *filename, string output, string authmsg); // Wrapped function, used in main(), do all the passwork check and data sending/receiving work

int main ( int argc, char **argv)
{  int accept_sd; 
   string authmsg;
   string output;
   
   /* argument usage */
    if(argc!=3){
    cout<<"usage: a.out <Port Number> <Password File Name>"<<endl;
    return -1;}
    cout << "running....\n";
    
   /* Check whether input file exists*/
     if( fexist(argv[2]))
        cout << "Password file opened."<< endl;
     else{
        cout << "File does not exist! Please try again." << endl;
        return -1;
        }

      // Create the socket
       ServerSocket server (atoi(argv[1]));
       cout << "Server is ready!" <<endl; 
       cout << "Waiting on accept()" << endl;
      
      /*Server ready to accept requests, it keeps listening until being terminated by the user*/
      while(true){ 
	    ServerSocket new_sock;
	    server.accept ( new_sock );   
	    cout << "Accept() is completed successfully! Waiting for data from clients." << endl;
        
        /*processing authentication here*/
        dostuff(new_sock, argv[2], output, authmsg); 
        
        /*authentication done. close socket connection to the specific client*/               
        new_sock.~ServerSocket(); 
	} // end of while-loop
  return 0;
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
                message and status of authentication. The input file uses tab '\t' as the delimiter
                for user name and password pair on each line. Each line only stores one pair. We 
                process the password file line by line to check.
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
{           int number=0;
            while(number<3){
                   std::string data1;
                   std::string data2;

		           new_sock >> data1; //read user name
                   new_sock >> data2; // read password

                  ifstream infile;
                  infile.open(filename, ifstream::in);
                  string inbuf;
                  bool user_exist = false;                  
                  bool auth_success = false;          
                      
                  output = checkPassword(infile,inbuf,data1, data2, output, user_exist,auth_success);
                  closeFile(infile);
                  
                  if(!user_exist) 
                      output = "User name not exist. Please try again.";
                      
                  /*Check if 3 failures happens, if yes, then notify client and close connection*/
                  if(!auth_success){
                      number++;
                      if(number>=3){
                         authmsg = "exit";
                         cout << "3 failures, exit." << endl;}
                      else
                         authmsg = output;
                      }
                  else{
                      authmsg = "Success";
                      new_sock << authmsg;
                      break;}                  
                  new_sock << authmsg; // write authentication result into the buffer and send
               } // end of while-loop   
}
