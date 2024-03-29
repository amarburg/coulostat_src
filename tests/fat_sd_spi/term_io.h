#ifndef TERM_IO_H_
#define TERM_IO_H_

#include "integer.h"
#include "comm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xgetc() (char)comm_get()
#define xavail() comm_test()

static inline void xputc_endl( void ) { xputc('\r'); xputc('\n'); }
#define ENDL "\r\n"

int xatoi (char**, long*);
void xputs (const char*);
void xitoa (long, int, int);
void xprintf (const char*, ...) __attribute__ ((format (__printf__, 1, 2)));
void put_dump (const byte_t *, 
               DWORD ofs, 
               int cnt);
void get_line (char*, int len);
int get_line_r (char*, int len, int *idx);

#ifdef __cplusplus
}
#endif
#endif /* TERM_IO_H_ */
