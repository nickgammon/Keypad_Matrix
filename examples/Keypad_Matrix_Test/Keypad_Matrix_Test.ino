/*
 * 
 * Example of using Keypad_Matrix with a 4x4 keypad matrix.
 * 
 */

#include <Keypad_Matrix.h>

const byte ROWS = 4;
const byte COLS = 4;

// how the keypad has its keys laid out
const char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};

const byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
const byte colPins[COLS] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad

  // Create the Keypad
Keypad_Matrix kpd = Keypad_Matrix( makeKeymap (keys), rowPins, colPins, ROWS, COLS );

void keyDown (const char which)
  {
  Serial.print (F("Key down: "));
  Serial.println (which);
  if (which >= '0' && which <= '9')
    {
    if (kpd.isKeyDown ('A'))
      Serial.println ("A is down as well.");
    if (kpd.isKeyDown ('B'))
      Serial.println ("B is down as well.");
    if (kpd.isKeyDown ('C'))
      Serial.println ("C is down as well.");
    if (kpd.isKeyDown ('D'))
      Serial.println ("D is down as well.");
    }
  }

void keyUp (const char which)
  {
  Serial.print (F("Key up: "));
  Serial.println (which);
  }


void setup() 
{
  Serial.begin (115200);
  Serial.println ("Starting.");
  kpd.begin ();
  kpd.setKeyDownHandler (keyDown);
  kpd.setKeyUpHandler   (keyUp);
}

void loop() 
{
  kpd.scan ();
}
