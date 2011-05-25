#ifndef __TERM_IO_H__
#define __TERM_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

  // This is something of the reverse sense of "extern "C"" from usual.
  // This defines these C++ functions as using C linkage ... so
  // they can be called from C code.
  void debug_print( const char *c );
  void debug_println( const char *c );
  char debug_get( void );
  void debug_putc( char c );

#ifdef __cplusplus
}
#endif

#endif
