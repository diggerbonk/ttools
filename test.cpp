//
// File     : $Id$
// Author   : Trent McNair
//
// This file contains various tests for the ttools library.

#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "tt_buffer.h"
#include "tt_socket.h"
#include "tt_async_socket.h"
#include "tt_notify.h"
#include "tt_listener.h"
#include "tt_network.h"
#include "tt_linked_list.h"
#include "tt_functions.h"
#include "tt_mutex.h"
#include "tt_hashtable.h"

const int TT_TEST_ECHOSERVER = 11;
const int TT_TEST_FT = 9;

using namespace std;

TTNetwork * ttnetwork;
TTMutex * mutex;
time_t starttime;
time_t endtime;
int test_type = 0;
int total_bytes = 0;
bool done = false;

class MyNotify : public TTNotify {
public:
   void DoNotify(long int channel, int type, void * data);
};

void MyNotify::DoNotify(long int channel, int type, void * data)
{
   if ( type == TT_NOTIFY_CONNECTED) {
      cout << "TT_NOTIFY_CONNECTED on channel " << channel << endl;
   }
   else if ( type == TT_NOTIFY_END) {
      cout << "TT_NOTIFY_END from " << channel << endl;
      done = true;
   }
   else if ( type == TT_NOTIFY_IN) {
      
      TTBuffer * ttb = (TTBuffer*)data;
      
#ifdef DEBUG
      cout << "Data in on channel " << channel << ", size " << ttb->Size() << endl;
#endif

      if ( test_type == TT_TEST_ECHOSERVER ) {
         ttnetwork->Send(channel,ttb->Buffer(), ttb->Size());
      }
      else if ( test_type == TT_TEST_FT ) {
         mutex->Lock();
         total_bytes += ttb->Size();
         mutex->Unlock();
      }
      
      ttb->Pop(ttb->Size());
   }
   else if ( type == TT_NOTIFY_ACCEPT) {
      cout << "TT_NOTIFY_ACCEPT from " << channel << endl;
   }
}

void TestThreadedSocket()
{
}

void TestTTSocket()
{
   TTSocket * sock = new TTSocket();
   sock->Connect("192.168.1.5", 80, 10);
   sock->Send((const unsigned char*)"GET http://basement/homepgs/index.php\n", 38);
   unsigned char buf[512];
   memset(buf,0,512);
   while ( sock->Recv(buf,256,10) > 0 ) {
      cout << buf << endl;
      memset(buf,0,512);
   }
}

void TestServer(int port)
{
   mutex = new TTMutex();
   TTNotify * ttn = new MyNotify();
   ttnetwork = new TTNetwork(ttn);
   starttime = time(NULL);
   total_bytes = 0;
   ttnetwork->Listen(NULL,port);

   while ( true ) sleep(1);
}

void TestNetwork(char * argv[])
{
   // args : prog network ip port number size
   int port = atoi(argv[3]);
   int max = atoi(argv[4]);
   int bufsize = atoi(argv[5]);
   
   TTNotify * notify = new MyNotify();
   ttnetwork = new TTNetwork(notify);
   mutex = new TTMutex();
   
   unsigned char buffer[512];
   memset(buffer, 0 , 512);
   
   long int c1;
   starttime = time(NULL);
   for (int i=0;i<max;i++) {
      c1 = ttnetwork->Connect(argv[2], port);
      ttnetwork->Send(c1, buffer, bufsize);
      usleep(100000);
   }
   
   while ( true ) sleep(1);
}


void TestLinkedList()
{
   TTLinkedList * ttl = new TTLinkedList();
   
   char * me = new char [strlen("trent")+1];
   strcpy(me,"trent");
   
   ttl->Insert((void*)me);
   ttl->Insert((void*)me);
   ttl->Insert((void*)me);
   ttl->Insert((void*)me);
   ttl->Insert((void*)me);
   
   TTLinkedList * temp = ttl;
   
   while ( temp->next ) {
      cout << "adslkfj" << endl;
      temp = temp->next;
   }
}

void TestMaxConnections(char * argv[])
{
   // args : prog max ip port number
   int port = atoi(argv[3]);
   int max = atoi(argv[4]);
   
   TTNotify * notify = new MyNotify();
   ttnetwork = new TTNetwork(notify);
   mutex = new TTMutex();
   
   unsigned char buffer[512];
   memset(buffer, 0 , 512);
   
   long int c1;
   starttime = time(NULL);
   for (int i=0;i<max;i++) {
      c1 = ttnetwork->Connect(argv[2], port);
      ttnetwork->Send(c1, buffer, 5);
      usleep(50000);
   }
   
   while ( true ) sleep(1);
}

//
// Test the speed of the hash table approach.

void TestHash(char * argv[])
{
   // program hash size
   
   starttime = time(NULL);
   TTHashtable * ht = new TTHashtable();
   int entries = atoi(argv[2]);
   int iterations = atoi(argv[3]);
   
   cout << "Testing TTHashtable." << endl;
   cout << "   Entries    : " << entries << endl;
   cout << "   Iterations : " << iterations << endl;
   
   // enter the values into the hash table
   
   char buf[16];
   strcpy(buf,"ll");
   for ( int i=0;i<entries;i++ ) {
      ht->Put((long int)i,buf);
   }
   
   starttime = time(NULL);
   for ( int i=0;i<iterations;i++ ) {
      for ( int j=0;j<entries;j++ ) {
//         if ( ht->Get((long int)j) ) {
//            cout << "found " <<  j << endl;
//         }
      }
   }
   
   cout << "   Time       : " << (time(NULL) - starttime) << endl;
   cout << "Done Testing TTHashtable." << endl << endl;
}

//
// Test the speed of the linked list approach.

void TestList(char * argv[])
{
   TTLinkedList * list = new TTLinkedList();
   TTLinkedList * temp;
   int entries = atoi(argv[2]);
   int iterations = atoi(argv[3]);
   int  intlist[1000];
   
   cout << "Testing TTLinkedList." << endl;
   cout << "   Entries    : " << entries << endl;
   cout << "   Iterations : " << iterations << endl;
   
   // enter the values into the linked list
   
   char buf[16];
   for ( int i=0;i<entries;i++ ) {
      sprintf(buf, "%i", i);
      intlist[i] = i;
      list->Insert(&intlist[i]);
   }
   
   starttime = time(NULL);
   for ( int i=0;i<iterations;i++ ) {
      for ( int j=0;j<entries;j++ ) {
         sprintf(buf, "%i", j);
         temp = list;
         while ( temp->next ) {
            if ( *(int*)temp->next->item == j ) {
               //cout << "found " << j << endl;
               break;
            }
            temp = temp->next;
         }
      }
   }
   
   cout << "   Time       : " << (time(NULL) - starttime) << endl;
   cout << "Done Testing TTLinkedList." << endl << endl;
}

//
// Test memory useage when doing a lot of allocation / deallocation

void TestMemory()
{
   TTNotify * notify = new MyNotify();
   ttnetwork = new TTNetwork(notify);
   mutex = new TTMutex();
   long int channels[10];
   unsigned char buffer[16];
   memset(buffer,0,16);
   
   ttnetwork->Listen(NULL,2541);
   
   int i;
   while ( true ) {
      // connect, send
      for (i=0;i<5;i++) {
         channels[i] = ttnetwork->Connect("127.0.0.1", 2541);
         usleep(100000);
         ttnetwork->Send(channels[i], buffer, 16);
      }
      
      sleep(2);
      
      // disconnect
      
      for (i=0;i<5;i++) {
         ttnetwork->Disconnect(channels[i]);
      }
      
      sleep(2);
      
   }
}

void BigTest(char * argv[])
{
   // args : prog big ip port number size
   int port = atoi(argv[3]);
   int max = atoi(argv[4]);
   int bufsize = atoi(argv[5]);
   
   TTNotify * notify = new MyNotify();
   ttnetwork = new TTNetwork(notify);
   mutex = new TTMutex();
   ttnetwork->Listen(NULL,port);
   
   unsigned char * buffer = new unsigned char[bufsize];
   memset(buffer, 0 , bufsize);
   
   long int c1;
   starttime = time(NULL);
   for (int i=0;i<max;i++) {
      c1 = ttnetwork->Connect(argv[2], port);
      ttnetwork->Send(c1, buffer, bufsize);
      usleep(100000);
   }
   
   while ( true ) sleep(1);
}

void SendFile(char * argv[])
{
   // testapp sendfile host port filename
   int port = atoi(argv[3]);
   char * fileName = argv[4];
   char * host = argv[2];
   total_bytes = 0;
   
   TTNotify * notify = new MyNotify();
   ttnetwork = new TTNetwork(notify);
   mutex = new TTMutex();
   
   long int c1 = ttnetwork->Connect(host,port);
   
   ifstream inFile;
   inFile.open(fileName, ios::in);
   if ( !inFile ) {
      cout << "Couldn't open file" << endl;
      exit(0);
   }

   // open the file for reading.
   starttime = time(NULL);
   unsigned char buf[2048];
   int totalBytes = 0;
   while ( inFile ) {
      inFile.read((char*)buf,2048);
      if ( inFile ) {
         ttnetwork->Send(c1,buf,2048);
         totalBytes += 2048;
      }
   }
   
   ttnetwork->Disconnect(c1);
   
   cout << "File transfer " << totalBytes << " bytes in ";
   cout << (time(NULL) - starttime) << " seconds" << endl;
   
}

int main ( int argc, char * argv[] ) 
{
   if ( strcmp(argv[1],"server") == 0 ) {
      test_type = 1;
      TestServer(atoi(argv[2]));
   }
   else if ( strcmp(argv[1],"network") == 0 ) {
      test_type = 3;
      TestNetwork(argv);
   }
   else if ( strcmp(argv[1], "max") == 0 ) {
      // args : prog max ip port number
      test_type = 5;
      TestMaxConnections(argv);
   }
   else if ( strcmp(argv[1], "hash") == 0 ) {
      test_type = 6;
      TestHash(argv);
   }
   else if ( strcmp(argv[1], "list") == 0 ) {
      test_type = 7;
      TestList(argv);
   }
   else if ( strcmp(argv[1], "memory") == 0 ) {
      test_type = 7;
      TestMemory();
   }
   else if ( strcmp(argv[1], "big") == 0 ) {
      test_type = 8;
      BigTest(argv);
   }
   else if ( strcmp(argv[1], "sendfile") == 0 ) {
      test_type = TT_TEST_FT;
      SendFile(argv);
   }
   else if ( strcmp(argv[1], "echoserver") == 0 ) {
      test_type = TT_TEST_ECHOSERVER;
      TestServer(atoi(argv[2]));
   }
}


