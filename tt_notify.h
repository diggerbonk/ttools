//
// File     : $Id$
// Author   : Trent McNair
//
// Interface class to implement a notification callback.

#ifndef __tt_notify_h
#define __tt_notify_h

#define TT_NOTIFY_BEGIN 1
#define TT_NOTIFY_CONNECTED 2
#define TT_NOTIFY_END 3
#define TT_NOTIFY_IN 4
#define TT_NOTIFY_ACCEPT 5
#define TT_NOTIFY_ERROR 6

class TTBuffer;
class TTSocket;

class TTNotify
{
public:

   virtual ~TTNotify() {};
   
   // the DoNotify callback. Remember that this callback happens 
   // in the socket's thread-space so take appropriate cautions.  

   void Notify(long int pChannel, int pType, void * pData);

protected:

   virtual void DoNotify(long int channel, int type, void * data) = 0;
   void NotifyLater(long int pChannel, int pType, void * pData);
};

#endif // __tt_notify_h
   
