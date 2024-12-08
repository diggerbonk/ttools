//
// File     : $Id$
// Author   : Trent McNair
//
// Set of functions used in the tt library.

#include <iostream>
#include <cstring>
#include <stdio.h> // for sprintf
#include <stdarg.h> // for char * fmt ...

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

void TT_Slice()
{
#ifdef WIN32
   // windows sleep routine is milliseconds (1/1000)
   Sleep(1);
#else
   // unix type systems usleep is microseconds (1/1,000,000)
   usleep(1);
#endif
}

//
// Converts a short packed in a buffer to an unsigned short 
// integer.

unsigned short TT_ShortFromBuffer(unsigned char * buffer, int offset)
{
   return (unsigned short)((buffer[offset]<<8) + buffer[offset+1]);
}

//
// TT_StringFromBuffer
//
// Extracts a string from a buffer, returns an allocated, 
// null-terminated char string. Caller is responsible for 
// de-allocation.

char * TT_StringFromBuffer(unsigned char * buf, int offset, int len)
{
   char * str = new char[len + 1];
   memcpy(str,buf,len);
   str[len] = '\0';
   return str;
}

//
// TT_IntegerToString
//
// Returns a string created from an integer.  Memory allocated must 
// be deleted by the caller.

char * TT_IntegerToString(int num)
{
   char tempStr[16];
   sprintf(tempStr, "%i", num);
   char * newStr = new char[strlen(tempStr) + 1];
   strcpy(newStr, tempStr);
   return newStr;
}

//
// TT_Debug

void TT_Debug(char * fmt, ... )
{
#ifdef DEBUG
   char message[2048];
   va_list list;
   va_start(list, fmt);
   vsprintf(message, fmt, list);
   va_end(list);  
   std::cout << message << std::endl;
#endif
}

//
// TT_Debug

void TT_Error( char * fmt, ... )
{
   char message[2048];
   va_list list;
   va_start(list, fmt);
   vsprintf(message, fmt, list);
   va_end(list);  
   std::cout << message << std::endl;
}
