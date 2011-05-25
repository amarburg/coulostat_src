// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "main.h"
#include "libmaple.h"
#include "wirish.h"

#include "sd_power.h"


enum ConsoleState_t {
  CONSOLE_TOP
};
static ConsoleState_t consoleState = CONSOLE_TOP;

// Various menu callbacks
bool test1_do( void )
{
  return true;
}

void console_init( void )
{
}

void print_help(void) 
{
  COMM.println("");
  //COMM.println("Command Listing\t(# means any digit)");
  COMM.println("Command Listing");
  COMM.println("\t?: print this menu");
  COMM.println("\th: print this menu");
  COMM.println("\tw: print Hello World on all 3 USARTS");
}


void console_splash( void )
{
  /* Send a message out over COMM interface */
  COMM.println(" ");
  COMM.println("    __   __             _      _");
  COMM.println("   |  \\/  | __ _ _ __ | | ___| |"); 
  COMM.println("   | |\\/| |/ _` | '_ \\| |/ _ \\ |");
  COMM.println("   | |  | | (_| | |_) | |  __/_|");
  COMM.println("   |_|  |_|\\__,_| .__/|_|\\___(_)");
  COMM.println("                 |_|");
  COMM.println("                              by leaflabs");
  COMM.println("");
  COMM.println("");
  COMM.println("Maple interactive test program (type '?' for help)");
  COMM.println("------------------------------------------------------------");
  COMM.print("> ");

}

static void consoleTop( uint8 input )
{

  switch(input) {
    case 13:  // Carriage Return
      COMM.println("what?");
      break;
    case 32:  // ' '
      COMM.println("spacebar, nice!");
      break;
    case 63:  // '?'
    case 104: // 'h'
      print_help();
      break;
    case 117: // 'u'
      SerialUSB.println("Hello World!");
      break;
    case 119: // 'w'
      Serial1.println("Hello World!");
      Serial3.println("Hello World!");
      break;
    case 95:  // '_'
      COMM.println("Delaying for 5 seconds...");
      delay(5000);
      break;
    case 'p':
      COMM.println("Toggling SD card power ...");
      if( sd_chk_power() == true ) 
        COMM.println("SD power is on, turning off" );
      else
        COMM.println("SD power is off, turning on" );
      sd_toggle_power();
      /* Wait for settling */
      delay(100);
      if( sd_chk_power() == true ) 
        COMM.println("SD power is on");
      else
        COMM.println("SD power is off");
      break;
    default:
      COMM.print("Unexpected: ");
      COMM.println(input);
  }
  COMM.print("> ");
}


void do_console( void ) 
{
  uint8 input;

  while(COMM.available()) {
    input = COMM.read();
    COMM.println(input);


    switch( consoleState ) {
      case CONSOLE_TOP:
        consoleTop( input );
        break;
      default:
        break;
    }
  }
}


