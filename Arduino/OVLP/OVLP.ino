/* ----------------------------------------------------------------------------------------------
   include keyboard library
   ----------------------------------------------------------------------------------------------
*/
#include "Keyboard.h"


/* ----------------------------------------------------------------------------------------------
   define pin <-> button mappings
   ----------------------------------------------------------------------------------------------
*/
#define button_Speed 5
#define button_Loop 6
#define button_Back 7
#define button_PlayPause 8
#define button_Forward 9


/* ----------------------------------------------------------------------------------------------
   global variables
   ----------------------------------------------------------------------------------------------
*/
long longPressTime = 1250;        // how long is a long press in msec?

bool active_Speed = false;        // speed button press active?
bool active_Speed_LP = false;     // speed button long-press active?
long timer_Speed = 0;             // speed button long-press timer

bool active_Loop = false;         // loop button press active?
bool active_Loop_LP = false;      // loop button long-press active?
long timer_Loop = 0;              // loop button long-press timer

bool active_Back = false;         // back button press active?
bool active_Back_LP = false;      // back button long-press active?
long timer_Back = 0;              // back button long-press timer

bool active_PlayPause = false;    // play/pause button press active?
bool active_PlayPause_LP = false; // play/pause button long-press active?
long timer_PlayPause = 0;         // play/pause button long-press timer

bool active_Forward = false;      // forward button press active?
bool active_Forward_LP = false;   // forward button long-press active?
long timer_Forward = 0;           // forward button long-press timer


/* ----------------------------------------------------------------------------------------------
   press and release a combination of keys via USB HID
   ----------------------------------------------------------------------------------------------
*/
void pressKeys(bool ctrl, bool shift, bool alt, char c) {
  // press ctrl if necessary
  if (ctrl) {
    Keyboard.press(KEY_LEFT_CTRL);
  }

  // press shift if necessary
  if (shift) {
    Keyboard.press(KEY_LEFT_SHIFT);
  }

  // press alt if necessary
  if (alt) {
    Keyboard.press(KEY_LEFT_ALT);
  }

  // press character and release after 100ms
  Keyboard.press(c);
  delay(100);
  Keyboard.release(c);

  // release all other keys
  Keyboard.releaseAll();
}


/* ----------------------------------------------------------------------------------------------
   setup
   ----------------------------------------------------------------------------------------------
*/
void setup() {
  // setup pins as inputs
  pinMode(button_Speed, INPUT);
  pinMode(button_Loop, INPUT);
  pinMode(button_Back, INPUT);
  pinMode(button_PlayPause, INPUT);
  pinMode(button_Forward, INPUT);

  // start USB HID interface
  Keyboard.begin();
}


/* ----------------------------------------------------------------------------------------------
   loop
   ----------------------------------------------------------------------------------------------
*/
void loop() {

  /* ----------------------------------------------------------------------------------------------
     BUTTON: SPEED
     ----------------------------------------------------------------------------------------------
  */

  // button pressed
  if (digitalRead(button_Speed) == HIGH) {

    // has not been pressed before => start timer
    if (active_Speed == false) {
      active_Speed = true;
      timer_Speed = millis();
    }

    // this is a long press
    if ((millis() - timer_Speed > longPressTime) && (active_Speed_LP == false)) {
      active_Speed_LP = true;
      // do long-press action: nothing
    }
  }

  // button not pressed anymore
  else if (active_Speed == true) {
    active_Speed = false;

    // this was a short press
    if (active_Speed_LP == false) {
      // do short-press action: Shift+Alt+E
      pressKeys(false, true, true, 'e');
    }

    // this was a long press
    else {
      active_Speed_LP = false;
    }
  }


  /* ----------------------------------------------------------------------------------------------
     BUTTON: LOOP
     ----------------------------------------------------------------------------------------------
  */

  // button pressed
  if (digitalRead(button_Loop) == HIGH) {

    // has not been pressed before => start timer
    if (active_Loop == false) {
      active_Loop = true;
      timer_Loop = millis();
    }

    // this is a long press
    if ((millis() - timer_Loop > longPressTime) && (active_Loop_LP == false)) {
      active_Loop_LP = true;
      // do long-press action: nothing
    }
  }

  // button not pressed anymore
  else if (active_Loop == true) {
    active_Loop = false;

    // this was a short press
    if (active_Loop_LP == false) {
      // do short-press action: Shift+Alt+W
      pressKeys(false, true, true, 'w');
    }

    // this was a long press
    else {
      active_Loop_LP = false;
    }
  }


  /* ----------------------------------------------------------------------------------------------
     BUTTON: BACK
     ----------------------------------------------------------------------------------------------
  */

  // button pressed
  if (digitalRead(button_Back) == HIGH) {

    // has not been pressed before => start timer
    if (active_Back == false) {
      active_Back = true;
      timer_Back = millis();
    }

    // this is a long press
    if ((millis() - timer_Back > longPressTime) && (active_Back_LP == false)) {
      active_Back_LP = true;
      // do long-press action: Shift+Alt+R
      pressKeys(false, true, true, 'r');
    }
  }

  // button not pressed anymore
  else if (active_Back == true) {
    active_Back = false;

    // this was a short press
    if (active_Back_LP == false) {
      // do short-press action: Shift+Alt+A
      pressKeys(false, true, true, 'a');
    }

    // this was a long press
    else {
      active_Back_LP = false;
    }
  }


  /* ----------------------------------------------------------------------------------------------
     BUTTON: PLAY/PAUSE
     ----------------------------------------------------------------------------------------------
  */

  // button pressed
  if (digitalRead(button_PlayPause) == HIGH) {

    // has not been pressed before => start timer
    if (active_PlayPause == false) {
      active_PlayPause = true;
      timer_PlayPause = millis();
    }

    // this is a long press
    if ((millis() - timer_PlayPause > longPressTime) && (active_PlayPause_LP == false)) {
      active_PlayPause_LP = true;
      // do long-press action: Shift+Alt+Q
      pressKeys(false, true, true, 'q');
    }
  }

  // button not pressed anymore
  else if (active_PlayPause == true) {
    active_PlayPause = false;

    // this was a short press
    if (active_PlayPause_LP == false) {
      // do short-press action: Shift+Alt+S
      pressKeys(false, true, true, 's');
    }

    // this was a long press
    else {
      active_PlayPause_LP = false;
    }
  }


  /* ----------------------------------------------------------------------------------------------
     BUTTON: FORWARD
     ----------------------------------------------------------------------------------------------
  */

  // button pressed
  if (digitalRead(button_Forward) == HIGH) {

    // has not been pressed before => start timer
    if (active_Forward == false) {
      active_Forward = true;
      timer_Forward = millis();
    }

    // this is a long press
    if ((millis() - timer_Forward > longPressTime) && (active_Forward_LP == false)) {
      active_Forward_LP = true;
      // do long-press action: nothing
    }
  }

  // button not pressed anymore
  else if (active_Forward == true) {
    active_Forward = false;

    // this was a short press
    if (active_Forward_LP == false) {
      // do short-press action: Shift+Alt+D
      pressKeys(false, true, true, 'd');
    }

    // this was a long press
    else {
      active_Forward_LP = false;
    }
  }


}
