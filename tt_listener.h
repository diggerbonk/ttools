//
// File     : $Id$
// Author   : Trent McNair
//
// TTListener - threaded network listener.  Creates 
// a thread that listens for incoming network connections, 
// assigns them a TTSocket, and performs a callback.
// The callback must take the allocated TTSocket and 
// do *something* with it.  The callback handler is 
// responsible for de-allocating the TTSocket.
//
// Part of the TTools package.

#ifndef __tt_listener_h
#define __tt_listener_h

#ifdef WIN32
#else
#include <pthread.h>
#endif

class TTNotify;
class TTSocket;
class TTMutex;

class TTListener {

public:

   TTListener(TTNotify * ttn);
   ~TTListener();
   
   void Start(char * ifc, int prt);
   void Stop();
   

protected:

   void Run();
      
private:

   static void * EntryPoint(void *);

   bool running;
   bool stop;
   int port;
   char * interface;
   TTNotify * notify;
   pthread_t thread_id;
   TTMutex * mutex;
};

#endif
