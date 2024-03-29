// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"
#include "term_io.h"
#include <stdint.h>

#include "rtc/rtc.h"
#include "rtc/ds1338.h"

#define LED_PIN 13
#define PWM_PIN  2

uint8_t input = 0;
uint8_t tiddle = 0;
int toggle = 0;
int rate = 0;
int sample = 0;



Ds1338Rtc rtc( Ds1338Rtc::I2C_1 );

void print_help(void);

// Not entirely happy with dedicating a variable to this.  Don't need to do 
// this if I take over the init function.
bool init_complete = false;

void setup() {

  /* Set up the LED to blink  */
  pinMode(LED_PIN, OUTPUT);

  /* Start up the serial ports */
  Serial1.begin(115200);
  Serial1.println("Test test");
  //    Serial3.begin(9600);

  /* Send a message out over COMM interface */
  COMM.println("");
  COMM.println("DS1338 Test program.");
  COMM.println("------------------------------------------------------------");
  COMM.print("> ");

 Serial1.println("Finished with setup");
  init_complete = true;
}

void readRam( void )
{
  uint8_t ram[ DS1338_RAM_SIZE ];
  int retval = 0;
uint8_t c  = 0,d = 0;
  
  COMM.println("Querying ram...");
  retval = rtc.readRam( 0, DS1338_RAM_SIZE, ram );
  
  if( retval == DS1338_RAM_SIZE ) {
    for( int i = 0; i < DS1338_RAM_SIZE; i++ ) {
      c = (ram[i] % 16) + '0';
      if( c > '9' ) c+= 7;
      COMM.print(c);
      d = (ram[i] - (ram[i]%16)) + '0';
      if( d > '9' ) d+= 7;
      COMM.print(d);
      COMM.print(' ');

      if( (i>0) && ((i%8)==0) ) COMM.println("");

    }
  } else {
    COMM.println("Didn't get the expected return value ");
    COMM.print( retval );
    COMM.println();
  }

}

void writeRam( void )
{

}

void readDate( void )
{
  Rtc::RawTime_t time;

  COMM.println("Querying RTC");
  rtc.readTimeRaw( time );
  COMM.print("Sec: ");
  COMM.println( time.sec );

  COMM.print("Min: ");
  COMM.println( time.min );

  COMM.print("Hour: ");
  COMM.println( time.hour );

  COMM.print("Day of week: ");
  switch( time.dow ) {
    case 0: COMM.println("Sunday"); break;
    case 1: COMM.println("Monday"); break;
    case 2: COMM.println("Tuesday"); break;
    case 3: COMM.println("Wednesday"); break;
    case 4: COMM.println("Thursday`"); break;
    case 5: COMM.println("Friday"); break;
    case 6: COMM.println("Saturday"); break;
  }

  COMM.print("Date: ");
  COMM.print( time.date );
  switch( time.month ) {
    case 0: COMM.print(" January "); break;
    case 1: COMM.print(" Febuary "); break;
    case 2: COMM.print(" March "); break;
    case 3: COMM.print(" April "); break;
    case 4: COMM.print(" May "); break;
    case 5: COMM.print(" June "); break;
    case 6: COMM.print(" July "); break;
    case 7: COMM.print(" August "); break;
    case 8: COMM.print(" September "); break;
    case 9: COMM.print(" October "); break;
    case 10: COMM.print(" November "); break;
    case 11: COMM.print(" December "); break;
    default: COMM.print(" undefined! "); break;
  }
  COMM.println( time.year );

}

void setDate( void )
{
  Rtc::RawTime_t time;

  time.sec = 10;
  time.min = 10;
  time.hour = 10;
  time.dow = 2;
  time.date = 2;
  time.month = 2;
  time.year = 11;

  COMM.println("Setting date");
  rtc.writeTimeRaw( time );
}


void loop() {

  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);

  delay(100);

  while(COMM.available()) {
    input = COMM.read();
    COMM.println(input);
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
      case 'r':
        // Read contents of ram
        readRam();
        break;
      case 'w':
        // Write to ram
        writeRam();
        break;
      case 'q':
        // Query if clock is running
        COMM.println("Clock is ");
        if( rtc.isClockRunning() )
          COMM.println(" running");
        else
          COMM.println(" not running");
        break; 
      default:
          COMM.print("Unexpected: ");
        COMM.println(input);
    }
    COMM.print("> ");
  }
}

void print_help(void) {
  COMM.println("");
  //COMM.println("Command Listing\t(# means any digit)");
  COMM.println("Command Listing");
  COMM.println("\t?: print this menu");
  COMM.println("\th: print this menu");
  COMM.println("\tw: print Hello World on all 3 USARTS");
}


// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated object that need libmaple may fail.
__attribute__(( constructor )) void premain() {
  init();
}

int main(void)
{
  setup();

  while (1) {
    loop();
  }
  return 0;
}
