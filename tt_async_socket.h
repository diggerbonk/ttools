//
// File     : $Id$
// Author   : Trent McNair
//
// The TTAsyncSocket2 class provides a single asynchronous socket.  All calls 
// return immediately and are followed by a callback in the sockets own 
// thread context.  Synchronizing is up to the caller, but the socket is 
// reentrant and synchronized.
//
// The socket is implemented with a single worker thread doing the reads, 
// writes are non-blocking and happen in the caller's context.
//
// The socket is a use-once then throw away model.
//
// The thread ascends through 5 states: ready, connecting, connected, 
// closing, done.  In some cases, the states connecting and connected are 
// skipped.

#ifndef __tt_async_socket_h
#define __tt_async_socket_h

#include <pthread.h>

class TTBuffer;
class TTSemaphore;
class TTMutex;
class TTNotify;
class TTSocket;

const int TTAS_STATUS_READY = 0;
const int TTAS_STATUS_CONNECTING = 1;
const int TTAS_STATUS_CONNECTED = 2;
const int TTAS_STATUS_STOPPED = 3;
const int TTAS_STATUS_CLOSED = 4;

class TTAsyncSocket {

public:

   TTAsyncSocket(TTNotify * tn, long int id);
   ~TTAsyncSocket();

   bool Connect(char * hst, int prt);
   bool Connect(TTSocket * sk);
   bool Disconnect();
   
   bool Send(unsigned char * buf, int len);
   void ReadThread();
   long int ID(){return id;}
   long int Status(){return status;}

private:
   void Stop();
   
   int status;
   char * host;
   int port;
   TTSocket * sock;
   TTBuffer * inbuf;
   TTBuffer * outbuf;
   TTNotify * notify;
   TTMutex * mutex;
   pthread_t read_thread_id;
   long int id;
};

#endif
