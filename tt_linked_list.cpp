//
// File     : $Id$
// Author   : Trent McNair
//
// Implements a linked list, with the usual linked list operations.

#include <cstddef>

#include "ttools/tt_linked_list.h"

TTLinkedList::TTLinkedList()
{
   item = NULL;
   next = NULL;
}

TTLinkedList::TTLinkedList(void * itm)
{
   next = NULL;
   item = itm;
}

TTLinkedList::~TTLinkedList()
{
}

//
// Insert an item in the front of the list.

void TTLinkedList::Insert(void * itm)
{
   TTLinkedList * ll = new TTLinkedList(itm);
   ll->next = next;
   next = ll;
}

//
// Pop the first item off the list, return a
// a pointer to the item.  If no item, 
// return NULL

TTLinkedList * TTLinkedList::Pop()
{
   if ( !next ) return NULL;
   else {
      TTLinkedList * tempPtr = next;
      next = next->next;
      return tempPtr;
   }
}
