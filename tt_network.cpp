//
// File     : $Id$
// Author   : Trent McNair
//
// TTNetwork - class represents a network - a group of connected 
// sockets and network listeners.  This class can be used to 
// implement robust servers.

#include <cstddef>
#include <iostream>

using namespace std;

#include "ttools/tt_listener.h"
#include "ttools/tt_network.h"
#include "ttools/tt_hashtable.h"
#include "ttools/tt_socket.h"
#include "ttools/tt_async_socket.h"
#include "ttools/tt_functions.h"
#include "ttools/tt_mutex.h"
#include "ttools/tt_linked_list.h"

//
// This is the notify callback from the socket and listener 
// threads.  We filter it here because we want to handle 
// the listerner callbacks, and we want to do a mutex (if 
// specified by the user) then notify the user.

void TTNetwork::DoNotify(long int channel, int type, void * data)
{
   // need create a socket and add it to the list of 
   // sockets
   
   if ( type == TT_NOTIFY_ACCEPT ) {
      TT_Debug("TTNetwork::DoNotify TT_NOTIFY_ACCEPT");
      channel_source++;
      TTAsyncSocket * ttas = new TTAsyncSocket(this,channel_source);
      DoCleanup();
      mutex->Lock();
      sockets->Put(channel_source,(void*)ttas);
      mutex->Unlock();
      ttas->Connect((TTSocket*)data);
   }
   else if ( type == TT_NOTIFY_END ) {
      // this socket is ready to be removed from the list
      notify->Notify(channel,type, data);
   }
   else {
      TT_Debug("TTNetwork::DoNotify Other notification");
      notify->Notify(channel,type, data);
   }
}

TTNetwork::TTNetwork(TTNotify * ttn)
{
   TT_Debug("TTNetwork::TTNetwork");
   notify = ttn;
   sockets = new TTHashtable(521);
   listener = new TTListener(this);
   channel_source = 0;
   mutex = new TTMutex();
}

TTNetwork::~TTNetwork()
{
   TT_Debug("TTNetwork::~TTNetwork");
   // TODO : Deallocate each item in the sockets list.
   delete sockets;
   delete listener;
}

//
// Connect a socket to the given host an port. A TTNotify 
// will be sent upon successful connect or connect failure.

long int TTNetwork::Connect(char * host, int port)
{
   channel_source++;
   TTAsyncSocket * ttas = new TTAsyncSocket(this,channel_source);
   DoCleanup();
   mutex->Lock();
   sockets->Put(channel_source,(void*)ttas);
   mutex->Unlock();
   ttas->Connect(host,port);
   return channel_source;
}

//
// Disconnect a channel socket

void TTNetwork::Disconnect(long int chn)
{
   cout << "Disconnect channel0 " << chn << endl;
   // Find the socket and send the data.
   mutex->Lock();
   cout << "Disconnect channel1 " << chn << endl;
   TTAsyncSocket * ttas = NULL;
   TT_Debug("TTNetwork::Disconnect");
   
   ttas = (TTAsyncSocket*)sockets->Get(chn);
   if ( ttas ) {
      cout << "Disconnect channel2 " << chn << endl;
      ttas->Disconnect();
      cout << "Disconnect channel3 " << chn << endl;
   }
   cout << "Disconnect channel4 " << chn << endl;
   mutex->Unlock();
}

//
// Remove a disconnected socket, only called internally.

void TTNetwork::Remove(long int chn)
{
   cout << "Disconnect channel " << chn << endl;
   // Find the socket and send the data.
   mutex->Lock();
   TTAsyncSocket * ttas = NULL;
   
   ttas = (TTAsyncSocket*)sockets->Get(chn);
   if ( ttas ) {
      sockets->Remove(chn);
      delete ttas;
   }
   mutex->Unlock();
}

void TTNetwork::Listen(char * interface, int port)
{
   listener->Start(interface,port);
}

void TTNetwork::ListenStop(int port)
{
   listener->Stop();
}

//
// Disconnect all connections, stop listeners, 
// and deallocate memory associated with them.

void TTNetwork::ShutdownNetwork()
{
   // STOP THE LISTENER(S)
   
   listener->Stop();
   
   // DISCONNECT EACH SOCKET
   
   mutex->Lock();
   
   TTLinkedList * ttl = sockets->Enumerate();
   HashBucket * hb;
   TTAsyncSocket * ts;
   
   if ( ttl ) {
      ttl = ttl->next;
      while ( ttl ) {
         hb = (HashBucket*)ttl->item;
         ts = (TTAsyncSocket*)hb->value;
         if ( ts ) ts->Disconnect();
         ttl = ttl->next;
      }
   }
   
   mutex->Unlock();   
}

//
// Send some data on the given channel.

bool TTNetwork::Send(long int channel, unsigned char * data, int dataLen)
{
   TT_Debug("TTNetwork::Send 1");
   TTAsyncSocket * ttas = NULL;
   
   // Find the socket and send the data.
   mutex->Lock();
   ttas = (TTAsyncSocket*)sockets->Get(channel);
   if ( ttas ) {
      ttas->Send(data,dataLen);
      mutex->Unlock();
      TT_Debug("TTNetwork::Send Sending data from TTNetwork::Send");
      return true;
   }
   mutex->Unlock();
   return false;
}

//
// Cleanup any sockets that are marked as done.  This should be 
// called regularly (perhapse each time a new socket is allocated) 
// to cleanup unused memory and threads.

void TTNetwork::DoCleanup()
{
   mutex->Lock();
   TTLinkedList * ttl = sockets->Enumerate();
   HashBucket * hb;
   TTAsyncSocket * ts;
   
   if ( ttl ) {
      ttl = ttl->next;
      while ( ttl ) {
         hb = (HashBucket*)ttl->item;
         ts = (TTAsyncSocket*)hb->value;
         if ( ts && ts->Status() == TTAS_STATUS_CLOSED ) {
            sockets->Remove(hb->ikey);
            delete ts;
         }
         ttl = ttl->next;
      }
   }
   
   mutex->Unlock();
}
