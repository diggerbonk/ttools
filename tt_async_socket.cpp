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


#include <string.h>
#include <iostream>

#include "ttools/tt_socket.h"
#include "ttools/tt_async_socket.h"
#include "ttools/tt_semaphore.h"
#include "ttools/tt_buffer.h"
#include "ttools/tt_functions.h"
#include "ttools/tt_notify.h"
#include "ttools/tt_mutex.h"

using namespace std;

//
// Create an async socket.  
//
//    nt : object to receive network notifications.
//    id : socket id.  The caller can assign a unique id 
//         for each socket.  The id number will be referenced 
//         in all network notifications.

TTAsyncSocket::TTAsyncSocket(TTNotify * nt, long int pid)
{
   notify = nt;
   inbuf = new TTBuffer();
   outbuf = new TTBuffer();
   sock = NULL;
   port = 0;
   host = NULL;
   status = TTAS_STATUS_READY;
   id = pid;
   mutex = new TTMutex();
}

TTAsyncSocket::~TTAsyncSocket()
{
#ifdef DEBUG
   cout << "TTAsyncSocket::~TTAsyncSocket" << endl;
#endif
   Stop();
   delete [] host;
   delete sock;
   delete inbuf;
   delete outbuf;
}

//
// This is the thread stuff.

#ifdef WIN32
#else
void * TTSocketReadThread( void * parm ) {
    ((TTAsyncSocket*)parm)->ReadThread();
    return 0;
}
#endif

//
// Start
//
// Start the threaded socket using an already-connected 
// TTSocket.

bool TTAsyncSocket::Connect(TTSocket * tsock)
{
   // we're doing a test and set on the status here, lock it 
   // with the mutex.
   mutex->Lock();
   if ( status != TTAS_STATUS_READY ) {
      mutex->Unlock();
      return false;
   }
   else {
      status = TTAS_STATUS_CONNECTING;
      mutex->Unlock();
      TT_Debug("TTAsyncSocket::Start - called with existing socket");
      sock = tsock;
#ifdef WIN32  
#else
      pthread_create (&read_thread_id, NULL, TTSocketReadThread, (void*)this);
#endif
      return true;
   }
}

//
// Start
//
// Start the socket, connect to phost/pport.  This call 
// initiates a thread, and a handler needs to be setup 
// prior to calling this.

bool TTAsyncSocket::Connect(char * phost, int pport)
{
   // we're doing a test and set on the status here, lock it 
   // with the mutex.
   mutex->Lock();
   if ( status != TTAS_STATUS_READY ) {
      mutex->Unlock();
      return false;
   }
   else {
      status = TTAS_STATUS_CONNECTING;
      mutex->Unlock();
      TT_Debug("TTAsyncSocket::Start - called");
      host = new char[strlen(phost)+1];
      strcpy(host,phost);
      port = pport;
#ifdef WIN32  
#else
      pthread_create (&read_thread_id, NULL, TTSocketReadThread, (void*)this);
#endif
      return true;
   }
}

//
// Gracefully disconnect the socket. This call is asynchronous, 
// a notification will be sent when the socket is finished 
// disconnecting.  Returns true if 

bool TTAsyncSocket::Disconnect()
{
   // test and set of status
   mutex->Lock();
   if ( status == TTAS_STATUS_READY || status == TTAS_STATUS_CLOSED ) {
      mutex->Unlock();
      return false;
   }
   else {
      status = TTAS_STATUS_STOPPED;
      mutex->Unlock();
      return true;
   }
}

//
// Stop the socket thread and wait for it to finish. 

void TTAsyncSocket::Stop()
{
   if ( Disconnect() ) pthread_join(read_thread_id, NULL);
}

//
// Send data on the socket.  Mutexed.

bool TTAsyncSocket::Send(unsigned char * buf, int len)
{
   mutex->Lock();   
   if ( status < 2 ) {
      // if we're not connected yet, we allow the data to be pipelined 
      // for a later send.  There is no guarantee that it will be sent, 
      // but a copy of the queued buffer will be sent on a failure notification 
      // so the caller can retrieve the data if they wish.
      outbuf->Add(buf,len);
      mutex->Unlock();
      return true;
   }
   else if ( status == 2 ) {
      // call send directly.
      int retVal = sock->Send(buf, len);
      if ( retVal < 0 ) {
         mutex->Unlock();
         Disconnect();
         return false;
      }
      else {
         mutex->Unlock();
         return true;
      }
   }
   else {
      mutex->Unlock();
      return false;
   }
}  

void TTAsyncSocket::ReadThread()
{
   // notify our owner that we are connecting now.
   notify->Notify(id, TT_NOTIFY_BEGIN, NULL);
   
   // connect the socket, if we fail, notify the user and 
   // quit.
   if ( sock == NULL ) {
      // connect the socket.
      sock = new TTSocket();
      if ( sock->Connect(host,port,10) ) {
         TT_Debug("TTAsyncSocket::ReadThread Connect worked");
      }
      else {
         TT_Debug("TTAsyncSocket::ReadThread Connect Failed");
         status = TTAS_STATUS_CLOSED;
         notify->Notify(id, TT_NOTIFY_END, NULL);
         return;
      }
   }
   else {
      // TODO: Test sock
   }
   
   // if there's waiting data in the out buffer, send it now.
   mutex->Lock();
   if ( outbuf->Size() > 0 ) {
      if ( sock->Send((const unsigned char*)outbuf->Buffer(), outbuf->Size()) < 0 ) {
         TT_Debug("TTAsyncSocket::ReadThread() Pre-send failed.");
         status = TTAS_STATUS_CLOSED;
         mutex->Unlock();
         notify->Notify(id, TT_NOTIFY_END, NULL);
         return;
      }
   }
   status = TTAS_STATUS_CONNECTED;
   mutex->Unlock();
   notify->Notify(id,TT_NOTIFY_CONNECTED, NULL);
   unsigned char * buffer = new unsigned char[TT_MAX_WRITE];
   
         
   // go into read loop.
   int retVal = 0;
   while ( status == TTAS_STATUS_CONNECTED ) {
      retVal = sock->Recv(buffer, TT_MAX_WRITE, 10);
      if ( retVal < 0 ) {
         TT_Debug("TTAsyncSocket::ReadThread() Fail on RECV");
         break;
      }
      else if ( retVal > 0 ) {
         // add to our buffer and notify the owner.
         inbuf->Add(buffer, retVal);
         notify->Notify(id,TT_NOTIFY_IN, inbuf);
      }
      else if ( retVal == 0 ) {
         // 0 from TTSocket means a timeout occured, 
         // so we do nothing here
      }
   }
   delete [] buffer;
   status = TTAS_STATUS_CLOSED;
   notify->Notify(id, TT_NOTIFY_END, NULL);
}
