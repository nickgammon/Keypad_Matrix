/*

 Keypad_Matrix.h

 Author: Nick Gammon
 Date:   20 February 2018


 PERMISSION TO DISTRIBUTE

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.


 LIMITATION OF LIABILITY

 The software is provided "as is", without warranty of any kind, express or implied,
 including but not limited to the warranties of merchantability, fitness for a particular
 purpose and noninfringement. In no event shall the authors or copyright holders be liable
 for any claim, damages or other liability, whether in an action of contract,
 tort or otherwise, arising from, out of or in connection with the software
 or the use or other dealings in the software.

 --------------------------------

 Example of use:

  const byte ROWS = 4;
  const byte COLS = 4;

  // how the keypad has its keys laid out
  const char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
  };

  const byte colPins[COLS] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad
  const byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad

  // Create the Keypad (makeKeymap casts the keys array to const char *)
  Keypad_Matrix kpd = Keypad_Matrix (makeKeymap (keys), rowPins, colPins, ROWS, COLS );

  ...

  In setup, call "begin" to allocate memory, and set a keydown and/or a keyup handler:

  kpd.begin ();
  kpd.setKeyDownHandler (keyDown);

  ...

  In loop, call "scan" to check the current key status:

  void loop()
    {
    kpd.scan ();
    }

  ...

  The handlers return the keyvalue (from the "keys" array) corresponding to which key just went down or up. The handlers are
  called automatically for the appropriate keys during the call to "scan" above.


  void keyDown (const char which)
    {
    Serial.print (F("Key down: "));
    Serial.println (which);
    } // end of keyDown

  ...

  You can also query if other keys are currently down (eg. for doing something like Ctrl+C)

  if (isKeyDown ('*'))
    // do something

  ------

  The row handlers and read handler are intended to allow you to do your own reading of the columns. For example,
  you might use a 74HC165 to shift in 8 bits using SPI. You can omit those for conventional wiring to digital
  pins as default handlers are supplied.

  The read handler is called once for each column (passed as an argument the column pin from the colPins array). The default
    behaviour is to do a digital read of that column. The read handler should return LOW (pressed) or HIGH (not pressed).
  The start row handler is intended to prepare for reading the columns (for example, it might read the bits for each column in one operation). The default behaviour is to set that row to OUTPUT and LOW.
  The end row handlers is intended to wrap up after a row. The default behaviour is to set that row back to INPUT mode.

  For external hardware (like shift registers) you may want to pass "false" to enablePullups as that won't be relevant there.

*/

#include <Arduino.h>        // for Arduino stuff like data types, pinMode, etc.
#include <limits.h>         // for CHAR_BIT
#include <Keypad_Matrix.h>  // for class definition, etc.

// constructor
Keypad_Matrix::Keypad_Matrix (const char *keyMap, const uint8_t *rowPins, const uint8_t *colPins,
                              const uint8_t numRows, const uint8_t numCols, const bool enablePullups)
  : keyMap_ (keyMap), rowPins_ (rowPins), colPins_ (colPins), numRows_ (numRows), numCols_ (numCols), enablePullups_ (enablePullups)

      {
      totalKeys_        = numRows * numCols;
      lastKeySetting_  = NULL;
      lastKeyTime_     = NULL;
      debounce_Time_    = 10;  // milliseconds
      // no handlers yet
      keyDownHandler_   = NULL;
      keyUpHandler_     = NULL;
      startRowHandler_  = startRow;
      endRowHandler_    = endRow;
      // default to doing a digitalRead
      readHandler_    = digitalRead;
      } // end of Keypad_Matrix::Keypad_Matrix

Keypad_Matrix::~Keypad_Matrix ()
  {
  if (lastKeySetting_)
    free (lastKeySetting_);
  if (lastKeyTime_)
    free (lastKeyTime_);

  // set each column to back to input
  if (enablePullups_)
    for (uint8_t i = 0; i < numCols_; i++)
      pinMode (colPins_ [i], INPUT);

  } // end Keypad_Matrix::~Keypad_Matrix

void Keypad_Matrix::begin ()
  {
  // if begin() already called, don't allocate memory again
  if (lastKeySetting_ != NULL)
    return;

  // allocate one bit per key, rounded up to next byte
  lastKeySetting_ = (char *) calloc ((totalKeys_ + CHAR_BIT - 1) / CHAR_BIT, 1);
  // allocate an unsigned long for the time the key last changed, for each key
  if (lastKeySetting_ != NULL)
    lastKeyTime_  = (unsigned long *) calloc (totalKeys_, sizeof (unsigned long));

  // give up if we couldn't allocate memory for both arrays
  if (lastKeyTime_ == NULL)
    {
    if (lastKeySetting_)
      free (lastKeySetting_);
    return;
    }

  // set each column to input-pullup (optional)
  if (enablePullups_)
    for (uint8_t i = 0; i < numCols_; i++)
      pinMode (colPins_ [i], INPUT_PULLUP);

  } // end of Keypad_Matrix::begin

void Keypad_Matrix::scan ()
  {
  // if Keypad_Matrix::begin has not been called then memory hasn't been allocated for the arrays
  if (lastKeySetting_   == NULL ||
      startRowHandler_  == NULL ||  // we need these handlers
      endRowHandler_    == NULL ||
      readHandler_      == NULL)
    return;

  uint8_t keyNumber = 0;          // current key number
  unsigned long now = millis ();  // for debouncing
  char keyChanged [(totalKeys_ + CHAR_BIT - 1) / CHAR_BIT];  // remember which ones changed
  memset (keyChanged, 0, sizeof (keyChanged));    // nothing yet
  bool changed = false;                           // did anything change? Not yet.

  // check each row
  for (uint8_t row = 0; row < numRows_; row++)
    {
    // handle start of a row
    // default: set that row to OUTPUT and LOW
    startRowHandler_ (rowPins_ [row]);

    // check each column to see if the switch has driven that column LOW
    for (uint8_t col = 0; col < numCols_; col++)
      {
      // debounce - ignore if not enough time has elapsed since last change
      if (now - lastKeyTime_ [keyNumber] >= debounce_Time_)
        {
        bool keyState = readHandler_ (colPins_ [col]) == LOW; // true means pressed
        if (keyState != (BITTEST (lastKeySetting_, keyNumber) != 0)) // changed?
          {
          lastKeyTime_ [keyNumber] = now;  // remember time it changed
          changed = true;                  // remember something changed
          BITSET (keyChanged, keyNumber);  // remember this key changed

          // remember new state
          if (keyState)
            BITSET   (lastKeySetting_, keyNumber);
          else
            BITCLEAR (lastKeySetting_, keyNumber);
          }  // if key state has changed
        }  // debounce time up
      keyNumber++;
      } // end of for each column

    // handle end of a row
    // default: put row back to high-impedance (input)
    endRowHandler_ (rowPins_ [row]);
    }  // end of for each row

  // If anything changed call the handlers.
  // We do this now in case the keys aren't polled very frequently. We have now
  // detected all the changes (first) before calling any handlers, in case the
  // handler wants to know of combinations like Ctrl+Z.
  if (changed)
    {
    // do key-ups first to make sure that combinations handled by external code are correct
    for (uint8_t i = 0; i < totalKeys_; i++)
      {
      if (BITTEST (keyChanged, i) != 0)  // did this one change?
        {
         if (BITTEST (lastKeySetting_, i) == 0)  // is now up
           if (keyUpHandler_)
             keyUpHandler_ (keyMap_ [i]);
        }  // end of this key changed
      }  // end of for each key

    // now do key-downs
    for (uint8_t i = 0; i < totalKeys_; i++)
      {
      if (BITTEST (keyChanged, i) != 0)  // did this one change?
        {
         if (BITTEST (lastKeySetting_, i) != 0)  // is now down
           if (keyDownHandler_)
             keyDownHandler_ (keyMap_ [i]);
        }  // end of this key changed
      }  // end of for each key
    }  // end of something changed

  } // end of Keypad_Matrix::scan

bool Keypad_Matrix::isKeyDown (const char which)
  {
  // if Keypad_Matrix::begin has not been called then memory hasn't been allocated for the arrays
  if (lastKeySetting_ == NULL)
    return false;

  // locate the desired key by a linear scan
  //  - this is a bit inefficient, but for a 16-key keypad it will be pretty fast
  for (uint8_t i = 0; i < totalKeys_; i++)
    {
    if (keyMap_ [i] == which)
      return BITTEST (lastKeySetting_, i) != 0;  // true if down
    }
  return false;   // that key isn't known
  } // end of Keypad_Matrix::isKeyDown

// default handler for starting a row
void Keypad_Matrix::startRow (uint8_t rowPin)
  {
  // set that row to OUTPUT and LOW
  pinMode (rowPin, OUTPUT);
  digitalWrite (rowPin, LOW);
  } // end of Keypad_Matrix::startRow

// default handler for ending a row
void Keypad_Matrix::endRow (uint8_t rowPin)
  {
  // put row back to high-impedance (input)
  pinMode (rowPin, INPUT);
  } // end of Keypad_Matrix::startRow
