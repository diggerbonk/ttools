//
// File     : $Id$
// Author   : Trent McNair
//
// Hash table implementation.  Maps keys to values.  Keys are strings, 
// values are pointers - no memory management is done for the values, 
// if you delete a value from the table it is up to you to deallocate 
// any memory associated with the value.
//
// The table will not allow duplicate entries, an error condition is 
// returned if you try and add a duplicate key.
//
// Table size (the number of hash buckets) can be determined at run 
// time via a parameter passed to the constructor.  Prime number will 
// work best, here are some examples:
//
// 61 67 127 131 251 257 509 521 1021 1031 2039 2053 4093 4099

#include <iostream>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "ttools/tt_hashtable.h"
#include "ttools/tt_functions.h"
#include "ttools/tt_linked_list.h"

#ifdef DEBUG
int max_in_bucket = 0;
int total_max = 0;
#endif

using namespace std;

HashBucket::HashBucket(char * ky, void * vl)
{
   next = NULL;
   if ( ky ) {
      key = new char[strlen(ky)+1];
      strcpy(key,ky);
   }
   else key = NULL;
   ikey = -1;
   value = vl;
}

HashBucket::HashBucket(long int iky, void * vl)
{
   next = NULL;
   ikey = iky;
   key = NULL;
   value = vl;
}

HashBucket::~HashBucket()
{
   delete [] key;
}

//
// Put
//
// Put a new value in the hash table.  Don't add duplicate 
// keys, returns false if the key is already mapped in the 
// hashtable.  Keys are case sensative.

bool HashBucket::Put(char * ky, void * vl)
{
#ifdef DEBUG
   max_in_bucket++;
#endif
   if ( next == NULL ) {
      next = new HashBucket(ky,vl);
      return true;
   }
   else if ( next->key && strcmp(ky,next->key) == 0 ) {
      return false;
   }
   else return next->Put(ky,vl);
}

//
// Put
//
// Put a new value in the hash table.  Don't add duplicate 
// keys, returns false if the key is already mapped in the 
// hashtable.  Keys are case sensative.

bool HashBucket::Put(long int iky, void * vl)
{
#ifdef DEBUG
   max_in_bucket++;
#endif
   if ( next == NULL ) {
      next = new HashBucket(iky,vl);
      return true;
   }
   else if ( next->ikey == iky ) {
      return false;
   }
   else return next->Put(iky,vl);
}

//
// Remove
//
// Removes a mapping.  Returns a pointer to the 
// stored value, or null if nothing was there.

void * HashBucket::Remove(char * ky)
{
   if ( next == NULL ) return NULL;
   else if ( next->key && strcmp(ky,next->key) == 0 ) {
      void * valPtr = next->value;
      HashBucket * ptr = next;
      next = next->next;
      delete ptr;
      return valPtr;
   }
   else return next->Remove(ky);
}

//
// Remove
//
// Removes a mapping.  Returns a pointer to the 
// stored value, or null if nothing was there.

void * HashBucket::Remove(long int iky)
{
   if ( next == NULL ) return NULL;
   else if ( next->ikey == iky ) {
      void * valPtr = next->value;
      HashBucket * ptr = next;
      next = next->next;
      delete ptr;
      return valPtr;
   }
   else return next->Remove(iky);
}

//
// Get
//
// Returna  value from the hash table, returns NULL if 
// no mapping is found.

void * HashBucket::Get(char * ky)
{
   if ( next == NULL ) return NULL;
   else if ( next->key && strcmp(next->key,ky)==0 ) {
      return next->value;
   }
   else return next->Get(ky);
}

//
// Get
//
// Returna  value from the hash table, returns NULL if 
// no mapping is found.

void * HashBucket::Get(long int iky)
{
   if ( next == NULL ) return NULL;
   else if ( next->ikey == iky ) {
      return next->value;
   }
   else return next->Get(iky);
}

//
// Update
//
// Update the value in the bucket.  Returns null if nothing 
// was previously in the bucket, or returns the last value 
// if something was.

void * HashBucket::Update(char * ky, void * vl)
{
   if ( next == NULL ) return NULL;
   else if ( next->key && strcmp(next->key,ky)==0 ) {
      return next->value;
   }
   else return next->Get(ky);   
}

//
// Update
//
// Update the value in the bucket.  Returns null if nothing 
// was previously in the bucket, or returns the last value 
// if something was.

void * HashBucket::Update(long int iky, void * vl)
{
   if ( next == NULL ) return NULL;
   else if ( next->ikey == iky ) {
      return next->value;
   }
   else return next->Get(iky);   
}

//
// Clear
//
// Clears this list

void HashBucket::Clear()
{
   HashBucket * ptr;
   while ( next != NULL ) {
      ptr = next;
      next = next->next;
      delete(ptr);
   }
}

//
// Size
//
// Returns the size of the list.

int HashBucket::Size(int cnt)
{
   if ( next == NULL ) return cnt;
   else return next->Size(cnt+1);
}

TTHashtable::TTHashtable(int tableSize)
{
   table_size = tableSize;
   
   buckets = new HashBucket*[table_size];
   for ( int i = 0; i < table_size; i++ ) {
      buckets[i] = new HashBucket();
   }
   
   entries = 0;
}

TTHashtable::~TTHashtable()
{
   for ( int i = 0; i < table_size; i++ ) {
      buckets[i]->Clear();
      delete buckets[i];
   }
}

//
// Map a key to a value.

bool TTHashtable::Put(char * ky, void * vl)
{
#ifdef DEBUG
   max_in_bucket = 0;
#endif
   if ( buckets[Hash(ky)]->Put(ky,vl) ) {
      entries++;
#ifdef DEBUG
      if ( max_in_bucket > total_max ) {
         total_max = max_in_bucket;
         cout << "NEW MAX COLLISIONS: " << total_max << endl;
      }
#endif
      return true;
   }
   else return false;
}

//
// Map a key to a value.  If key is over MAXKEY size, then it will be 
// truncated.

bool TTHashtable::Put(long int iky, void * vl)
{
#ifdef DEBUG
   max_in_bucket = 0;
#endif
   if ( buckets[Hash(iky)]->Put(iky,vl) ) {
      entries++;
#ifdef DEBUG
      if ( max_in_bucket > total_max ) {
         total_max = max_in_bucket;
         cout << "NEW MAX COLLISIONS: " << total_max << endl;
      }
#endif
      return true;
   }
   else return false;
}

void  * TTHashtable::Get(char * ky)
{
   return buckets[Hash(ky)]->Get(ky);
}

void  * TTHashtable::Get(long int iky)
{
   return buckets[Hash(iky)]->Get(iky);
}

void * TTHashtable::Remove(char * ky)
{
   void * ptr ;
   if ( (ptr = buckets[Hash(ky)]->Remove(ky)) ) {
      entries--;
   }
   return ptr;
}

void * TTHashtable::Remove(long int iky)
{
   void * ptr ;
   if ( (ptr = buckets[Hash(iky)]->Remove(iky)) ) {
      entries--;
   }
   return ptr;
}

void TTHashtable::Clear()
{
   for ( int i = 0; i<table_size; i++ ) {
      buckets[i]->Clear();
   }
   entries = 0;
}

int TTHashtable::Size()
{
   return entries;
}

void * TTHashtable::Update(char * ky, void * vl)
{
   return buckets[Hash(ky)]->Update(ky,vl);
}

void * TTHashtable::Update(long int iky, void * vl)
{
   return buckets[Hash(iky)]->Update(iky,vl);
}

//
// Perform the hash.  This hash function was from a book 
// I owned at some point, I think maybe a Knuth book. I've been 
// using it forever.
//
// Trent McNair

int TTHashtable::Hash(char * s)
{
   char *p ; unsigned int h = 0, g ;

   for ( p = s ; *p != '\0' ; p++ ) {
      *p = (char)tolower(*p);
      h = (h << 4) + *p;
      if( (g = h & 0xf0000000) ) {
         h = h ^ (g >> 24) ;
         h = h ^ g ;
      }
   }
   return h % table_size;
}

int TTHashtable::Hash(long int iky)
{
   return (iky % table_size);
}

//
// returns the entire hashtable item set 
// in a linked list.

TTLinkedList * TTHashtable::Enumerate()
{
   TTLinkedList * ttl = new TTLinkedList();
   HashBucket * hb = NULL;
   
   for ( int i = 0; i<table_size; i++ ) {
      hb = buckets[i]->next;
      while ( hb ) {
         ttl->Insert((void*)hb->value);
         hb = hb->next;
      }
   }
   return ttl;
}
