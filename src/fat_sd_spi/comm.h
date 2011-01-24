#ifndef __COMM_H__
#define __COMM_H__

// Compatibility wrappers with Martin Thomas' term_io functions for now
void comm_println( const char * );

void comm_put( char c );
char comm_get( void );
int comm_test( void );


#endif

