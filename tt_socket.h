//
// File     : $Id$
// Author   : Trent McNair
//
// Socket wrapper, wrap a consistent interface around native 
// sockets.

#ifndef __tt_socket_h
#define __tt_socket_h

const int TT_MAX_READ = 1024;
const int TT_MAX_WRITE = 1024;

class TTSocket
{
public:

   TTSocket(int pSock);
   TTSocket();
   ~TTSocket();
   
   bool Listen(char * interface, int port);
   bool Connect(char * host, int port, int timeout);
   void Disconnect();
   int Send(const unsigned char * buffer, int len);
   int Recv(unsigned char * buffer, int max, int timeout);

private:

   unsigned long GetHostIP(char * host);
   int sock;
};

#endif // __tt_socket_h
