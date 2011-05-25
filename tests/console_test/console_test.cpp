
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "menu.h"

char ui_buf[80];
int ui_idx = 0;

const char *build_path( menu_t *menu, char *buf, int len )
{
  char sw[128];

  // Unoptimized
  strncpy( buf, menu->title, len );
  while( menu->parent ) {
    menu = menu->parent;
    strcpy( sw, buf );
    strncpy( buf, menu->title, len );
    strncat( buf, " ", len-strlen(buf)-1 );
    strncat( buf, sw, len-strlen(buf) );
  }

  return buf;
}

void prompt( void )
{
  char buf[128];
  printf("%s > %*s", build_path( get_current_menu(), buf, 127 ), ui_idx, ui_buf );
}

void parse_path( char *buf, int len )
{
// Buf will take the form of "Main foo bar baz"
// Destructively follow the paths until you get to a  leaf node
// Returns whatever remains in buf
}




int main( int argc, char **argv )
{
  int c;

  prompt();
  while( c = getchar() ) {
    // Special chars
    switch( c ) {
      case '\t':
        // parse the string so far and see if it makes sense

        break;
      case 0x08:  
        if(ui_idx > 0 ) {
          putchar( c );
          --ui_idx;
        }
        break;
      case 0x0A:
      case 0x0D:
        printf("Attempting to parse: \"%*s\"\n", ui_idx,ui_buf );

        ui_idx = 0;
        prompt();
        break;
      default:
        if( isprint( c ) ) {
          putchar(c);
          ui_buf[ui_idx++] = c;
        }
    }

  }
  return 0;
}


  // Callbacks

  bool test1_do( void )
  {
    printf("Test1!\n");
  }
