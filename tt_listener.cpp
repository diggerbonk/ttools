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

#include <string.h>

#ifdef WIN32
#else
#include <unistd.h> // for usleep
#include <sys/socket.h>
#include <arpa/inet.h>  // inet_addr and other net db functions
#endif

#include "ttools/tt_listener.h"
#include "ttools/tt_notify.h"
#include "ttools/tt_functions.h"
#include "ttools/tt_socket.h"
#include "ttools/tt_mutex.h"

TTListener::TTListener(TTNotify * ttn)
{
   thread_id = 0;
   stop = false;
   port = 0;
   interface = NULL;
   notify = ttn;
   running = false;
}

TTListener::~TTListener()
{
   Stop(); // stop the listner and collect the thread.
   delete [] interface;
}

//
// EntryPoint
//
// Private class where the thread is created.

void * TTListener::EntryPoint(void* param)
{
   ((TTListener*)param)->Run();
   return 0;
}

//
// Start the listening thread.  If a thread is already 
// running it will be killed.

void TTListener::Start(char * ifc, int prt)
{
   Stop();
   
   port = prt;
   if ( ifc ) {
      interface = new char[strlen(ifc)+1];
      strcpy(interface, ifc);
   }
   else interface = NULL;
   
   pthread_create(&thread_id, NULL, EntryPoint, (void*)this);
}

//
// Run
//
// Do the thread loop here.

void TTListener::Run()
{
   // first, set up the listening socket.
   int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
   if ( listenSocket < 0 ) {
      TT_Debug("TTListener::Run() error on listen socket allocation");
      stop = false;
      running = false;
      return;
   }
   
   // bind the socket to the specified port
   
   struct sockaddr_in sockAddr;
   memset(&sockAddr,0,sizeof(sockAddr));
   sockAddr.sin_family = AF_INET;
   if ( !interface ) sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   else sockAddr.sin_addr.s_addr = inet_addr(interface);
   sockAddr.sin_port = htons((u_short)port); 
   
   if(bind(listenSocket,(struct sockaddr *)&sockAddr, sizeof(sockAddr))) {
      running = false;
      stop = false;
      return;
   }
   
   
   int tempSock;
   struct sockaddr_in newAddr;
   socklen_t newAddrLen;
   memset(&newAddr,0,sizeof(newAddr));
   TTSocket * tsock = NULL;
   
   while ( true ) {
   
      if ( listen(listenSocket, 3) >= 0 ) {
         // we have a connection
         tempSock = accept(listenSocket,(struct sockaddr*) &newAddr, &newAddrLen);
         if ( tempSock >= 0 ) {
            tsock = new TTSocket(tempSock);
            notify->Notify(0,TT_NOTIFY_ACCEPT, (void*)tsock);
         }
      }
      else {
         // TODO: handle error condition well
         TT_Debug("TTListener::Run() error on listen, pausing...");
         usleep(10);
      }

      if ( stop ) break;
   }
   running = false;
}

void TTListener::Stop()
{
   // wait on the thread, if there is one.
   stop = true;
   if ( thread_id != 0 ) pthread_join(thread_id,NULL);
   running = false;
   thread_id = 0;
   stop = false;
}
