# Makefile for the socket programming example
#

simple_server_objects = ServerSocket.o Socket.o simple_server_main.o
simple_client_objects = ClientSocket.o Socket.o simple_client_main.o
simple_iterserver_objects = ServerSocket.o Socket.o simple_iterserver.o

all : simple_server simple_client simple_iterserver

simple_server: $(simple_server_objects)
	g++ -o ConcAuthServer $(simple_server_objects)


simple_client: $(simple_client_objects)
	g++ -o AuthClient $(simple_client_objects)

simple_iterserver: $(simple_iterserver_objects)
	g++ -o IterAuthServer $(simple_iterserver_objects)

Socket: Socket.cpp
ServerSocket: ServerSocket.cpp
ClientSocket: ClientSocket.cpp
simple_server_main: simple_server_main.cpp
simple_client_main: simple_client_main.cpp
simple_iterserver: simple_iterserver.cpp


clean:
	rm -f *.o ConcAuthServer AuthClient IterAuthServer
