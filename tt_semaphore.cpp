//
// File     : $Id$
// Author   : Trent McNair
//
// TTSemaphore - part of the ttools package.
//
// Implements a semaphore that wraps the underlying native semaphore 
// implementations (POSIX etc).  It is created to help portability 
// and to provide and easier to use interface.

#ifdef WIN32
#else
#endif

#include "ttools/tt_semaphore.h"
#include "ttools/tt_functions.h"

//
// TTSemaphore
//
// Constructor for class.  Initializes the semaphore, once it's 
// constructed, it can be used.

TTSemaphore::TTSemaphore(int cnt)
{
   count = cnt;
#ifdef WIN32
#else
   cond = new pthread_cond_t;
   mutex = new pthread_mutex_t;
   if ( pthread_mutex_init(mutex, NULL) == -1 ) {
      TT_Error("TTSemaphore::TTSemaphore mutex create failed");
   }

   if ( pthread_cond_init(cond, NULL) == -1 ) {
      TT_Error("TTSemaphore::TTSemaphore condition signal failed");
   }
#endif
}

TTSemaphore::~TTSemaphore()
{
#ifdef WIN32
#else
   if ( pthread_mutex_destroy(mutex) == -1 ) {
      TT_Error("TTSemaphore::~TTSemaphore destroy mutex");
   }

   if ( pthread_cond_destroy(cond) == -1 ) {
      TT_Error("TTSemaphore::~TTSemaphore destroy signal");
   }
   delete cond;
   delete mutex;
#endif
}

//
// Up
//
// Put the semaphore up.

int TTSemaphore::Up()
{
#ifdef WIN32
   return 0;
#else
   if ( pthread_mutex_lock(mutex) < 0 ) TT_Error("TTSemaphore::Up() pthread_mutex_lock");
   count++;
   int newCount = count;

   if ( pthread_mutex_unlock(mutex) < 0 ) TT_Error("TTSemaphore::Up() pthread_mutex_unlock");
   if ( pthread_cond_signal(cond) < 0 ) TT_Error("TTSemaphore::Up() pthread_cond_signal");

   return newCount;
#endif
}

//
// Down
//
// Put the semaphore down

int TTSemaphore::Down()
{
#ifdef WIN32
   return 0;
#else
   if ( pthread_mutex_lock(mutex) ) TT_Error("TTSemaphore::Down() pthread_mutex_unlock");
   while (  count <= 0 ) {
      if ( pthread_cond_wait(cond, mutex) ) TT_Error("TTSemaphore::Down() pthread_mutex_unlock");
   }

   count--;
   int newCount = count;

   if ( pthread_mutex_unlock(mutex) ) TT_Error("TTSemaphore::Down() pthread_mutex_unlock");

   return newCount;
#endif
}

