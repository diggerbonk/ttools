//
// File     : $Id$
// Author   : Trent McNair
//
// TTBuffer
//
// TTBuffer class implements a growing binary buffer.  Memory 
// allocation is done in chunks, and is deallocated when 
// popped off the beginning of the buffer.

#include <string.h>
#include <iostream>
#include <stdlib.h>

#ifdef WIN32
#else
#include <unistd.h>
#endif

#include "ttools/tt_buffer.h"
#include "ttools/tt_functions.h"

using namespace std;

TTBuffer::TTBuffer()
{
   allocated = 0;
   used = 0;
   buffer = NULL;
   read_index = 0;
}

TTBuffer::~TTBuffer()
{
   if ( buffer != NULL ) free(buffer);
}

void TTBuffer::Reset()
{
   if ( buffer != NULL ) free(buffer);
   allocated = 0;
   used = 0;
   buffer = NULL;
   read_index = 0;
}

bool TTBuffer::Add(const unsigned char * buf, int bufSize)
{
   if ( allocated == 0 ) {
      allocated = ((bufSize/TT_CHUNK_SIZE)+1)*TT_CHUNK_SIZE;
      buffer = (unsigned char*)malloc(allocated);
   }
   else if ( (used+bufSize) > allocated ) {
      allocated += (((bufSize/TT_CHUNK_SIZE)+1)*TT_CHUNK_SIZE);
      unsigned char * temp;
      temp = (unsigned char*)realloc(buffer, allocated);
      if ( temp == NULL ) {
         return false;
      }
      buffer = temp;
   }

   memcpy(buffer+(used),buf,bufSize);
   used+=bufSize;
   
   return true;
}

//
// Insert into an already allocated buffer, if not enough room  exists 
// in the current allocation, returns false.

bool TTBuffer::Insert(const unsigned char * buf, int bufSize, int offset)
{
   if ( (used + bufSize + offset) > allocated ) return false;
   memcpy(buffer+(used+offset),buf,bufSize);
   return true;
}

long int TTBuffer::Size()
{
   int retVal = ( used - read_index );
   return retVal;
}

//
// Returns a pointer to the internal buffer 

unsigned char * TTBuffer::Buffer()
{
   return buffer+(read_index);
}

bool TTBuffer::Pop(int popSize)
{
   if ( popSize <= 0 ) {
      return false;
   }

   read_index += popSize;
   
   if ( read_index >= allocated ) {
      free(buffer);
      buffer = NULL;
      read_index = 0;
      allocated = 0;
      used = 0;
   }
   
   return true;
}

//
// AddLong
//
// Add a long integer to the buffer (4 bytes)

bool TTBuffer::AddLong(long int number)
{
   unsigned char buf[4];

   buf[0] = (unsigned char)((number>>24) & 0x000000FF);
   buf[1] = (unsigned char)((number>>16) & 0x000000FF);
   buf[2] = (unsigned char)((number>>8) & 0x000000FF);
   buf[3] = (unsigned char)(number & 0x000000FF);

   return Add(buf,4);
}

//
// AddShort
//
// Add a short integer to the buffer (2 bytes)

bool TTBuffer::AddShort(short int number)
{
   unsigned char buf[2];

   buf[0] = (unsigned char)((number>>8) & 0x00FF);
   buf[1] = (unsigned char)(number & 0x00FF);

   return Add(buf, 2);
}


//
// AddByte
//
// Add a byte to the buffer (2 bytes)

bool TTBuffer::AddByte(unsigned char bt)
{
   unsigned char buf[1];
   buf[0] = (unsigned char)bt;
   return Add(buf, 1);
}

//
// AddString
//
// Adds a string to the end of the buffer, 
// If term is true, a null terminator will 
// also be appended to the buffer.

bool TTBuffer::AddString(const char * str, bool term)
{
   int len = strlen(str);
   if ( term ) len++;
   
   return Add((unsigned char*)str,len);  
}

//
// InsertShort
//
// Insert a short value at offset

bool TTBuffer::InsertShort(unsigned short number, int offset)
{
   if ( allocated >= (offset+2) ) {
      buffer[offset] = (unsigned char)((number>>8) & 0x00FF);
      buffer[offset+1] = (unsigned char)(number & 0x00FF);
      return true;
   }
   else {
      return false;
   }
}

//
// ShortFromBuffer
//
// Decodes a short within the buffer, returns 0 if 
// not enough buffer.

unsigned short TTBuffer::ShortFromBuffer(int offset)
{
   unsigned short num;
   if ( (2+offset+read_index) > used ) num = 0;
   else {
      num = TT_ShortFromBuffer(buffer+(read_index),offset);
   }
   return num;
}
