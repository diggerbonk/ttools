//
// File     : $Id$
// Author   : Trent McNair
//
// Interface class to implement a notification callback.

#include "ttools/tt_notify.h"

//
// Call this function to send a notification. 

void TTNotify::Notify(long int pChannel, int pType, void * pData)
{
   // if we're a gui, well call DoNotifyLater(), otherwise we just do 
   // the notification now, in this thread context.

   DoNotify(pChannel,pType,pData);
}

void TTNotify::NotifyLater(long int pChannel, int pType, void * pData)
{
   // push the packet onto the event queue of the given system.
}
