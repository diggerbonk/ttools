//
// File     : $Id$
// Author   : Trent McNair
//
// TTBuffer
//
// TTBuffer class implements a growing binary buffer.  Memory 
// allocation is done in chunks, and is deallocated when 
// popped off the beginning of the buffer.

const int TT_CHUNK_SIZE = 1024;

class TTBuffer {

public:

   TTBuffer();
   ~TTBuffer();
   
   bool Add(const unsigned char * buf, int bufSize);
   bool Insert(const unsigned char * buf, int bufSize, int offset);
   bool AddString(const char * str, bool);
   bool AddLong(long int);
   bool AddShort(short int);
   bool AddByte(unsigned char);

   bool Pop(int popSize);
   unsigned short ShortFromBuffer(int bytes);
   unsigned char * Buffer(); 
   
   long int Size(); // size of used bytes in buffer after read pointer.
   void Reset();
   
   bool InsertShort(unsigned short, int offset);
   
private:

   long int allocated;
   long int used;
   long int read_index;
   
   unsigned char * buffer;
};

