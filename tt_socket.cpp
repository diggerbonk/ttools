//
// File     : $Id$
// Author   : Trent McNair
//
// This class encapsulates the underlying native socket / network 
// method to aid in portability and provide a simpler programming 
// interface.

#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>  // inet_addr and other net db functions
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // for close()
#include <errno.h>
#endif

#include "ttools/tt_socket.h"
#include "ttools/tt_functions.h"

//
// Create a TTSocket object, providing an already-connected socket. 
// This may not be supported on all platforms.

TTSocket::TTSocket(int pSock)
{
   sock = pSock;
}

TTSocket::TTSocket()
{
   sock = -1;
}

TTSocket::~TTSocket()
{
   Disconnect();
}

///
/// Listen - listen for an incoming socket connection
///
/// If a connection comes in and is established, return 
/// true, otherwise false is returned.
///
/// interface is the name of the interface to listen on, 
/// if it is NULL, then we will listen on any available 
/// interface.

bool TTSocket::Listen(char * interface, int port)
{
   if ( sock >= 0 ) {
      return false;
   }
   
   int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
   if ( listenSocket < 0 ) {
      return false;
   }
   
   // bind the socket to the specified port
   
   struct sockaddr_in sockAddr;
   memset(&sockAddr,0,sizeof(sockAddr));
   sockAddr.sin_family = AF_INET;
   if ( !interface ) sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   else sockAddr.sin_addr.s_addr = inet_addr(interface);
   sockAddr.sin_port = htons((u_short)port);
   
   if(bind(listenSocket,(struct sockaddr *)&sockAddr, sizeof(sockAddr))) {
      return false;
   }

   // now wait for a connection
   
   if( listen(listenSocket,0) ) {
      return false;
   }

   // accept connection
   sock = accept(listenSocket,NULL,NULL);
   if ( sock < 0 ) {
      return false;
   }
#ifdef WIN32
   else closesocket(listenSocket);
#else
   else close(listenSocket);
#endif
   return true;      
}

//
// Connect - connect to remote host
//
// Connects the socket to a remote host, or returns false
// if a connection can not be made.  When returning 
// false, calling Error() will return a descriptive 
// error message as to why the connection failed.

bool TTSocket::Connect(char * host, int port, int timeout)
{
   if ( sock >= 0 ) {
      return false;
   }
   
   unsigned long cip = 0;
   if ( !host ) {
      return false;
   }
   else if ( port <= 0 ) {
      return false;
   }
   else if ( (cip = GetHostIP(host)) <= 0 ) {
      return false;
   }
   else if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
      return false;
   }
   
   struct sockaddr_in ladd;
   ladd.sin_family = AF_INET;
   ladd.sin_port = 0;
   ladd.sin_addr.s_addr = htonl(INADDR_ANY);
   
   if ( (bind(sock,(struct sockaddr*)&ladd,sizeof(ladd))) < 0 ) {
      return false;
   }
   
   struct sockaddr_in radd;
   radd.sin_family = AF_INET;
   radd.sin_port = htons(port);
   radd.sin_addr.s_addr = htonl(cip);
   
   if ( connect(sock,(struct sockaddr *)&radd, sizeof(radd)) < 0 ) {
      return false;
   }
   
   return true;   
}

//
// Send - send data on the socket connection.
//
// Returns one of three responses:
//
//  >0 : number of bytes sent.
//   0 : no bytes send ( no room in buffer )
//  <0 : error

int TTSocket::Send(const unsigned char * buff, int len)
{
   if ( sock < 0 ) {
      return -1;
   }
   
   // we'll send at most TT_MAX_WRITE bytes
   int bytesToSend = len;
   if ( bytesToSend > TT_MAX_WRITE ) bytesToSend = TT_MAX_WRITE;
   
   int retVal = send(sock,(const char*)buff,bytesToSend,0);
   if ( retVal < 0 ) {
      if ( errno == ENOBUFS ) {
         return 0;
      }
      else {
         Disconnect();
         return -1;
      }
   }
   else return retVal;
}

//
// Recv - recieve data from the socket connection.
//
// Returns one of three responses:
//
//    >0 : the number of bytes recieved, stored in buff
//    <0 : connection closed, see error message for reason.
//     0 : timeout, no data recieved

int TTSocket::Recv(unsigned char * buff, int len, int timeout)
{
   if ( sock < 0 ) {
      return -1;
   }
   
   fd_set rset;
   struct timeval tv;
   
   FD_ZERO(&rset);
   FD_SET(sock,&rset);
   tv.tv_sec = timeout;
   tv.tv_usec = 0;
   
   int retVal = select(sock+1,&rset,NULL,NULL,&tv);
   
   if ( retVal == 0 ) {
      return 0;
   }
   else if ( retVal < 0 ) {
      Disconnect();
      return -1;
   }
   else {
      retVal = recv(sock,buff,len,0);
      if ( retVal > 0 ) {
         TT_Debug("TTSocket::Recv() got some data");
         return retVal;
      }
      else if ( retVal == 0 ) {
         Disconnect();
         return -1;
      }
      else {
         Disconnect();
         return -1;
      }
   }
}

void TTSocket::Disconnect()
{
   if ( sock < 0 ) return;
   else {
      close(sock);
      sock = -1;
   }
}

//
// GetHostIp
//
// takes an interinet address (either in dotted
// quad or fqdn format) and returns the
// ip address in host byte order.
//
// returns the address or 0 if an error occurs.

unsigned long TTSocket::GetHostIP(char * addr)
{
   if ( inet_addr(addr) == INADDR_NONE ) {
      // not a valid dotted quad addr.
      struct hostent * heAddr = gethostbyname(addr);
      if ( !heAddr ) {
         return 0;
      }
      else {
         struct in_addr * iaddr = (in_addr*)heAddr->h_addr;
         return ntohl(iaddr->s_addr);
      }
   }
   else {
      return ntohl(inet_addr(addr));
   }
}
