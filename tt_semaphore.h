//
// File     : $Id$
// Author   : Trent McNair
//
// Implements a semaphore that wraps the underlying native semaphore 
// implementations (POSIX etc).  It is created to help portability 
// and to provide and easier to use interface.

#ifndef __tt_semaphore_h
#define __tt_semaphore_h

#ifdef WIN32
#else
#include <pthread.h>
#endif

class TTSemaphore {

public:

   TTSemaphore(int cnt = 1);
   ~TTSemaphore();
   
   int Up();
   int Down();
   
private:

   int count;
#ifdef WIN32
#else
   pthread_mutex_t * mutex;
   pthread_cond_t * cond;
#endif
};

#endif // __tt_semaphore_h
