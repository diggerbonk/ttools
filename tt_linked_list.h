//
// File     : $Id$
// Author   : Trent McNair
//
// Implements a linked list, with the usual linked list operations.

#ifndef __tt_linked_list_h
#define __tt_linked_list_h

class TTLinkedList {

public:

   TTLinkedList();
   TTLinkedList(void * itm);
   ~TTLinkedList();
   
   void Insert(void * itm);
   TTLinkedList * Pop();

   TTLinkedList * next;
   void * item;
};

#endif
   
   
