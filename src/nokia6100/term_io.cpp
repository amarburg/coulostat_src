
#include <wirish.h>
#include "term_io.h"

// Compatibility wrappers with Martin Thomas' term_io functions for now
void debug_println( const char *c )
{
  DEBUG.println(c);
}

char debug_get( void )
{
  uint8 b = DEBUG.read();
  return b;
}

int debug_available( void )
{
  return DEBUG.available();
}

void debug_putc( char c )
{
  DEBUG.write(c);
}

