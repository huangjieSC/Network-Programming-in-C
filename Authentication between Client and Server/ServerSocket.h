// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"


class ServerSocket : private Socket
{
 public:

  int index;

  ServerSocket ( int port );
  ServerSocket (){};
  ~ServerSocket(); 


  int getaccept_sd(); 

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  void accept ( ServerSocket& );
  int get_index(); 
};


#endif
