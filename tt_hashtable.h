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

#ifndef __tt_hashtable_h
#define __tt_hashtable_h

const int DEFAULT_HASH_TABLE_SIZE = 2039;

class TTLinkedList;

class HashBucket
{
public:
   HashBucket(char * ky = 0, void * vl = 0);
   HashBucket(long int ky, void * vl);
   ~HashBucket();

   bool Put(char * key, void * value);
   bool Put(long int key, void * value);
   void * Remove(char * key);
   void * Remove(long int key);
   void * Get(char * key);
   void * Get(long int key);
   void * Update(char * ky, void * vl);
   void * Update(long int ky, void * vl);
   void Clear();
   int Size(int cnt = 0);
   
   HashBucket * next;
   char * key;
   long int ikey;
   void * value;
};


class TTHashtable
{
public:
   TTHashtable(int tableSize = DEFAULT_HASH_TABLE_SIZE);
   ~TTHashtable();
   
   bool Put(char * key, void * value);
   bool Put(long int key, void * value);
   void * Get(char * key);
   void * Get(long int key);
   void * Remove(char * key);
   void * Remove(long int key);
   void * Update(char * ky, void * vl);
   void * Update(long int key, void * vl);
   void Clear();
   TTLinkedList * Enumerate();
   int Size();
   
private:
   int Hash(char * id);
   int Hash(long int id);
   int table_size;
   int entries;
   HashBucket ** buckets;
};

#endif



