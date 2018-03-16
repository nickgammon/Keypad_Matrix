// Author: Nick Gammon
// Date: 16 March 2018

#include <Keyboard.h>
#include <Keypad_Matrix.h>

const byte ROWS = 4;
const byte COLS = 4;

// what is on the keypad keys
char  keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' },
};

// how much to slow down keyboard output (so keys aren't missed)
const unsigned long DELAY_BETWEEN_KEYS = 10;

// keypad matrix wiring
byte rowPins[ROWS] = { 2, 3, 4, 5 };    // connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8, 9 }; // connect to the column pinouts of the keypad

// light the LED on key-down
const byte ledPin = 13;
// when we last lit the LED
unsigned long ledLit;

// helper macros to output strings with "F" macro around them
#define MSG(arg)                \
  do                            \
    myKeyboard.print (F (arg)); \
  while (false)

#define MSGLN(arg)                \
  do                              \
    myKeyboard.println (F (arg)); \
  while (false)

// do a newline easily
#define NEWLINE()       \
  do                       \
    myKeyboard.println (); \
  while (false)

// to avoid compiler warnings about unused arguments
#define UNUSED(expr) (void)(expr)

// Create the Keypad
Keypad_Matrix kpd = Keypad_Matrix (makeKeymap (keys), rowPins, colPins, ROWS, COLS);

// class for printing to our keyboard with a delay between each key
class my_Keyboard_class : public Print
{
  public:
  virtual size_t write (uint8_t c);
}; // end of class my_Keyboard_class

size_t my_Keyboard_class::write (uint8_t character)
{
  Keyboard.write (character);
  delay (DELAY_BETWEEN_KEYS);
  return 1;
} // end of my_Keyboard_class::write

my_Keyboard_class myKeyboard;

void setup ()
{
  // initialize control over the keyboard:
  Keyboard.begin ();

  // set up various pins
  pinMode (ledPin, OUTPUT);

  // initialize the keypad handler
  kpd.begin ();
  kpd.setKeyDownHandler (keyDown);
  kpd.setKeyUpHandler   (keyUp);

} // end of setup

// for things like multiple arrows, eg: specialKey (KEY_LEFT_ARROW, 2);
void specialKey (const uint8_t whichKey, const int n)
{
  for (int i = 0; i < n; i++)
    {
    Keyboard.press (whichKey);
    delay (5);
    Keyboard.releaseAll ();
    delay (5);
    } // end of for
} // end of specialKey

// messages for holding down A and then pressing a number, * or #
void do_A_messages (const char which)
{
  switch (which)
    {
    case '1': MSG ("Sending message for A+1"); break;
    case '2': MSG ("Sending message for A+2"); break;
    case '3': MSG ("Sending message for A+3"); break;
    case '4': MSG ("Sending message for A+4"); break;
    case '5': MSG ("Sending message for A+5"); break;
    case '6': MSG ("Sending message for A+6"); break;
    case '7': MSG ("Sending message for A+7"); break;
    case '8': MSG ("Sending message for A+8"); break;
    case '9': MSG ("Sending message for A+9"); break;
    case '0': MSG ("Sending message for A+0"); break;
    case '*': MSG ("Sending message for A+*"); break;
    case '#': MSG ("Sending message for A+#"); break;
    } // end of switching on which key
} // end of do_A_messages

// messages for holding down B and then pressing a number, * or #
// Note: Using MSGLN means each one will end with a newline
void do_B_messages (const char which)
{
  switch (which)
    {
    case '1': MSGLN ("Sending message for B+1"); break;
    case '2': MSGLN ("Sending message for B+2"); break;
    case '3': MSGLN ("Sending message for B+3"); break;
    case '4': MSGLN ("Sending message for B+4"); break;
    case '5': MSGLN ("Sending message for B+5"); break;
    case '6': MSGLN ("Sending message for B+6"); break;
    case '7': MSGLN ("Sending message for B+7"); break;
    case '8': MSGLN ("Sending message for B+8"); break;
    case '9': MSGLN ("Sending message for B+9"); break;
    case '0': MSGLN ("Sending message for B+0"); break;
    case '*': MSGLN ("Sending message for B+*"); break;
    case '#': MSGLN ("Sending message for B+#"); break;
    } // end of switching on which key
} // end of do_B_messages

// messages for holding down C and then pressing a number, * or #
void do_C_messages (const char which)
{
  switch (which)
    {
    case '1': MSG ("Sending message for C+1"); break;
    case '2': MSG ("Sending message for C+2"); break;
    case '3': MSG ("Sending message for C+3"); break;
    case '4': MSG ("Sending message for C+4"); break;
    case '5': MSG ("Sending message for C+5"); break;
    case '6': MSG ("Sending message for C+6"); break;
    case '7': MSG ("Sending message for C+7"); break;
    case '8': MSG ("Sending message for C+8"); break;
    case '9': MSG ("Sending message for C+9"); break;
    case '0': MSG ("Sending message for C+0"); break;
    case '*': MSG ("Sending message for C+*"); break;
    case '#': MSG ("Sending message for C+#"); break;
    } // end of switching on which key
} // end of do_C_messages

// messages for holding down D and then pressing a number, * or #
void do_D_messages (const char which)
{
  switch (which)
    {
    case '1': MSG ("Sending message for D+1"); break;
    case '2': MSG ("Sending message for D+2"); break;
    case '3': MSG ("Sending message for D+3"); break;
    case '4': MSG ("Sending message for D+4"); break;
    case '5': MSG ("Sending message for D+5"); break;
    case '6': MSG ("Sending message for D+6"); break;
    case '7': MSG ("Sending message for D+7"); break;
    case '8': MSG ("Sending message for D+8"); break;
    case '9': MSG ("Sending message for D+9"); break;
    case '0': MSG ("Sending message for D+0"); break;
    case '*': MSG ("Sending message for D+*"); break;
    case '#': MSG ("Sending message for D+#"); break;
    } // end of switching on which key

} // end of do_D_messages

// message for pressing a key with no modifier key
void do_unshifted_messages (const char which)
{
  switch (which)
    {
    case '1': MSG ("Sending message for 1"); break;
    case '2': MSG ("Sending message for 2"); break;
    case '3': MSG ("Sending message for 3"); break;
    case '4': MSG ("Sending message for 4"); break;
    case '5': MSG ("Sending message for 5"); break;
    case '6': MSG ("Sending message for 6"); break;
    case '7': MSG ("Sending message for 7"); break;
    case '8': MSG ("Sending message for 8"); break;
    case '9': MSG ("Sending message for 9"); break;
    case '0': MSG ("Sending message for 0"); break;
    case '*': MSG ("Sending message for *"); break;
    case '#': MSG ("Sending message for #"); break;
    } // end of switching on which key
} // end of do_unshifted_messages

// do action on key-up
void keyUp (const char which)
{
  if (kpd.isKeyDown ('A'))
    do_A_messages (which);
  else if (kpd.isKeyDown ('B'))
    do_B_messages (which);
  else if (kpd.isKeyDown ('C'))
    do_C_messages (which);
  else if (kpd.isKeyDown ('D'))
    do_D_messages (which);
  else
    do_unshifted_messages (which);
} // end of keyUp

// on key-down turn on the LED
void keyDown (const char which)
{
  UNUSED (which);
  digitalWrite (ledPin, HIGH);
  ledLit = millis ();
} // end of keyDown

void loop ()
{
  kpd.scan ();
  // turn LED off after 200 ms
  if (millis () - ledLit >= 200)
    digitalWrite (ledPin, LOW);
} // end of loop
