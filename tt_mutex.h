//
// File     : $Id$
// Author   : Trent McNair
//
// TTMutex - part of the ttools library.  This class is a wrapper 
// around the underlying systems native mutex functionality.  It 
// is provided to aid in cross-platform development and also to 
// provide a more simple interface.

#ifndef __tt_mutex_h
#define __tt_mutex_h

#ifdef WIN32
#else
#include <pthread.h>
#endif

class TTMutex
{
public:

   TTMutex();
   ~TTMutex();
   
   bool Lock();
   void Unlock();
   
private:

   pthread_mutex_t * mutex;
};

#endif // __tt_mutex_h
   
