Authentication between Client and Server

Name: Jie Huang



1. Programming enviroment
   This project was completed in Dev C++ and tested on the department linux machine 
   pluto.cse.sc.edu.

2. How to compile the programs.
   I have provided a Makefile along with the souce codes. Open the shell in the 
   linux machine and go to the folder with contains the source code, password file
   and Makefile. Then type the command "make" to compile the program and get the
   executable files. After you test the code, you can also use the command "make clean"
   to remove all the object files and executable files.

3. How to execute the programs.

Iterative version:
 
   <1> The name for the three executable files are
       Client side: AuthClient
       Iterative server side: IterAuthServer
       Concurrent server side: ConcAuthServer

   <2> First open a shell window and go to the folder which contains the executable files.
       Type in the command "./IterAuthServer <Port number> <Password file name>"
       An example would be "./IterAuthServer 55555 password.txt"

   <3> After the server side program has been successfully launched, open a new shell window
       and still go to the same folder as above. Type in the commad
       ./AuthClient <IP address> <Port number>
       An exmaple would be
       ./AuthClient 127.0.0.1 55555
       Note that we run the server and client on the same machine, so we use the local host
       address 127.0.0.1 as the IP address for the server and we must use the same port number
       that the server uses. Otherwise the connection could not be established.
   
   <4> If the client program is successfully launched, you should be able to see some messages
       prompt you for the user name and your password.After typing into the password, press 'Enter'
       to send data to the server and wait for the response.
   
   <5> If the user name and password match, client will receive the success message and exit the program.
   
   <6> If the user name and password do not match, client will get the failure message and be prompted for 
       the new input.

   <7> At maximum the client can try three success times, if they all fail, the connection to the server 
       will be closed and the user will be forced to exit. 

   <8> After the client exits, you can launch the client program again and start a new request. But each 
       time the server can only process one request at the same time.

   <9> The server program should be always on until you want to terminate it manually.

Concurrent version:

    <1> The name for the three executable files are
       Client side: AuthClient
       Iterative server side: IterAuthServer
       Concurrent server side: ConcAuthServer

   <2> First open a shell window and go to the folder which contains the executable files.
       Type in the command "./ConcAuthServer <Port number> <Password file name>"
       An example would be "./ConcAuthServer 55555 password.txt"

   <3> After the server side program has been successfully launched, open a new shell window
       and still go to the same folder as above. Type in the commad
       ./AuthClient <IP address> <Port number>
       An exmaple would be
       ./AuthClient 127.0.0.1 55555
       Note that we run the server and client on the same machine, so we use the local host
       address 127.0.0.1 as the IP address for the server and we must use the same port number
       that the server uses. Otherwise the connection could not be established.
   
   <4> If the client program is successfully launched, you should be able to see some messages
       prompt you for the user name and your password.After typing into the password, press 'Enter'
       to send data to the server and wait for the response.
   
   <5> If the user name and password match, client will receive the success message and exit the program.
   
   <6> If the user name and password do not match, client will get the failure message and be prompted for 
       the new input.

   <7> At maximum the client can try three success times, if they all fail, the connection to the server 
       will be closed and the user will be forced to exit. 

   <8> While the client program is running, you can launch another client program in a new shell window
       and start a new request. The server can process multiple request at the same time.

   <9> The server program should be always on until you want to terminate it manually.