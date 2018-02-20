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

*/

typedef void (*keyHandler) (const char which);
typedef int  (*colHandler) (uint8_t);
typedef void (*rowHandler) (uint8_t);

#define makeKeymap(x) ((const char*)x)  // for passing a multi-dimensional array to the constructor

class Keypad_Matrix {
  const char * keyMap_;     // array mapping key numbers to a character (eg. 0,0 is 'A')
  const uint8_t * rowPins_; // array of row pin numbers
  const uint8_t * colPins_; // array of column pin numbers
  uint8_t numRows_;
  uint8_t numCols_;
  bool enablePullups_;
  uint8_t totalKeys_;      // numRows_ * numCols_
  char * lastKeySetting_;  // bitmap of last setting of each key
  unsigned long * lastKeyTime_;   // when each key was last changed
  unsigned long debounce_Time_;   // how long to debounce for
  // event handlers (callback functions)
  keyHandler keyDownHandler_;  // handler for keydown event
  keyHandler keyUpHandler_;    // handler for keyup event
  rowHandler startRowHandler_; // handler called at the start of each row
  rowHandler endRowHandler_;   // handler called at the end of each row
  colHandler readHandler_;     // handler for reading a column

  // default handlers for starting and ending a row
  static void startRow (uint8_t rowPin);
  static void endRow   (uint8_t rowPin);

public:

  // constructor
  Keypad_Matrix (const char * keyMap, const uint8_t *rowPins, const uint8_t *colPins,
                 const uint8_t numRows, const uint8_t numCols, const bool enablePullups = true);
  // destructor
  ~Keypad_Matrix ();

  // public functions
  void begin ();        // call to initialize (set pullups, allocate memory)
  void scan ();         // call periodically to scan the keys
  bool isKeyDown            (const char which);  // see if a certain key is down
  void setKeyDownHandler    (keyHandler handler)  { keyDownHandler_   = handler; }  // handle key down
  void setKeyUpHandler      (keyHandler handler)  { keyUpHandler_     = handler; }  // handle key up
  void setRowHandlers       (rowHandler start, rowHandler end)
                              {
                              startRowHandler_  = start;
                              endRowHandler_    = end;
                              }  // row handlers
  void setColumnReadHandler (colHandler handler)  { readHandler_      = handler; }  // read a column
  void setDebounceTime      (const unsigned long debounce_Time) { debounce_Time_ = debounce_Time; }
};  // end of Keypad_Matrix class


// See: http://c-faq.com/misc/bitsets.html

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
