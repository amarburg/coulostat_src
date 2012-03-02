#ifndef __MAIN_H__
#define __MAIN_H__

// choose your weapon
#define COMM SerialUSB
#define DEBUG Serial1

#ifdef __cplusplus
extern "C" {
#endif

void return_to_main_menu( void );

// This is something of the reverse sense of "extern "C"" from usual.
// This defines these C++ functions as using C linkage ... so
// they can be called from C code.
void comm_println( const char *c );
char comm_get( void );
void xputc( char c );

void debug_println( const char *c );

void debug_led( unsigned int i );

#ifdef __cplusplus
}
#endif

#endif

