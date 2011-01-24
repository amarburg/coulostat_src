//
// Based on the Fatfs file integer.h 
/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

#include <stdint.h>

/* Apparently using all-caps for typedefs causes problems in C++
 * It's bad style in any case */

#define INT int
#define UINT unsigned int

/* These types must be 8-bit integer */
typedef uint8_t byte_t;
#define CHAR char
#define UCHAR unsigned char

/* These types must be 16-bit integer */
#define SHORT short
#define USHORT unsigned short
#define WORD unsigned short
#define WCHAR unsigned short

/* These types must be 32-bit integer */
#define LONG long
#define ULONG unsigned long
#define DWORD unsigned long

#endif

#endif
