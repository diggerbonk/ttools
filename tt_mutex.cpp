//
// File     : $Id$
// Author   : Trent McNair
//
// TTMutex - part of the ttools library.  This class is a wrapper 
// around the underlying systems native mutex functionality.  It 
// is provided to aid in cross-platform development and also to 
// provide a more simple interface.

#ifdef WIN32
#else
#endif

#include "ttools/tt_mutex.h"
#include "ttools/tt_functions.h"

TTMutex::TTMutex()
{
#ifdef WIN32
#else
   mutex = new pthread_mutex_t;
   if ( pthread_mutex_init(mutex, NULL) < 0 ) {
      TT_Error("TTMutex::TTMutex() TTMutex mutex create failed");
   }
#endif
}

TTMutex::~TTMutex()
{
#ifdef WIN32
#else
   if ( pthread_mutex_destroy(mutex) < 0 ) {
      TT_Error("TTMutex::~TTMutex() TTMutex destroy mutex failed");
   }
   delete mutex;
#endif
}

bool TTMutex::Lock()
{
#ifdef WIN32
   return false;
#else
   if ( pthread_mutex_lock(mutex) < 0 ) TT_Error("TTMutex::Lock() pthread_mutex_lock");
   return true;
#endif
}

void TTMutex::Unlock()
{
#ifdef WIN32
#else
   if ( pthread_mutex_unlock(mutex) < 0 ) TT_Error("TTMutex::Unlock() pthread_mutex_unlock");
#endif
}

