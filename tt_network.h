//
// File     : $Id$
// Author   : Trent McNair
//
// TTNetwork - class represents a network - a group of connected 
// sockets and network listeners.  This class can be used to 
// implement robust servers.

#ifndef __tt_network_h
#define __tt_network_h

#include "ttools/tt_notify.h"

class TTHashtable;
class TTListener;
class TTMutex;

class TTNetwork : public TTNotify {

public:

   TTNetwork(TTNotify * ttn);
   ~TTNetwork();
   
   long int Connect(char * host, int port);
   void Disconnect(long int chn);
   void Listen(char * interface, int port);
   void ListenStop(int port);
   void ShutdownNetwork();
   bool Send(long int channel, unsigned char * data, int dataLen);
   
   virtual void DoNotify(long int channel, int type, void * data);

protected:

   TTNotify * notify;
   
private:

   void Remove(long int channel);
   void DoCleanup();
   
   long int channel_source;
   TTHashtable * sockets;
   TTListener * listener;
   TTMutex * mutex;
};

#endif //__tt_network_h
   
