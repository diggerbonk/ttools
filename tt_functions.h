//
// File     : $Id$
// Author   : Trent McNair
//
// Set of functions used in the tt library.

#ifndef __tt_functions_h
#define __tt_functions_h

#define TT_CRITICAL 0
#define TT_WARNING 1
#define TT_INFO 2

void TT_Slice();

void TT_Debug(char * fmt, ...);
void TT_Error(char * fmt, ...);

char * TT_IntegerToString(int num);

unsigned short TT_ShortFromBuffer(unsigned char * buffer, int offset);
char * TT_StringFromBuffer(unsigned char * buffer, int offset, int len);

#endif
