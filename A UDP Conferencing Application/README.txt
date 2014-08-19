A UDP Conferencing Application

Name: Muhammad Nazmus Sakib, Jie Huang


1. Programming enviroment
   This project was completed in Dev C++ and tested on the department linux machine 
   pluto.cse.sc.edu, hebe.cse.sc.edu and hera.cse.sc.edu

2. How to compile the programs?
   To compile server program, use gcc confserver.c list.c -o confserver
   To compile client program, use gcc confclient.c list.c -o confclient
   After compilation,there should be two exectable file called 'confserver' and 'confclient'
   
3. How to run the programs?   
   The 'confserver' program takes  no command line arguments. 
   For example you can run it as follows. 

    % ./confserver
    Welcome to UDP conference server....
    UDP conference server is ready at hostname hebe and port 33211

    When a client connects to the server, the server will show the following messages:

    UDP Server is ready to get new connections from client...
    UDP Server received a join in message from host hera at port 58987, address is 129.252.130.173.
    single client: 129.252.130.173,58987,1

 
    The 'confclient'  program takes server host name and port as command
    line arguments. To connect to the above server, we can run

    % ./confclient hebe.cse.sc.edu 33211

    It will show messages like these:

    UDP conference Client is ready to connect....
    Connected to UDP server hebe.cse.sc.edu on port 33211.

    The first client would then show the current client list as follows:

    UDP client received the list of clients from 
    port 33211, address 129.252.130.149
    The current client list excluding yourself is:
    Client List Empty

   
    For the second client which was run on pluto.cse.sc.edu, it will show as follows:

    UDP client received the list of clients from 
    port 33211, address 129.252.130.149
    The current client list excluding yourself is:
    129.252.130.173,58987,1

    For the clients, whenever there is a new client joining in the conference, a message will be shown to
    all the current clients, message is like this:

    New Client joined: 129.252.130.171
 
    You should run the  server first and then many clients. Any thing typed by a
    client would appear at all other clients'. The message will be like:

    Message received from host hera at port 58987:
    hello!!!


4. How do i exit from these programs? 

   To exit from client, type  "exit" or Ctrl-D. To exit from server, just press Ctrl-C to kill it. 

5. Do these programs run on any machine? 

   These programs run on Linux machines only.
