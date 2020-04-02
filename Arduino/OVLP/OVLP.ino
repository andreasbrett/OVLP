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
#define button_Custom 10
#define button_Bank 11


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

bool active_Custom = false;       // custom button press active?
bool active_Custom_LP = false;    // custom button long-press active?
long timer_Custom = 0;            // custom button long-press timer

bool boolBankTwoSelected = false; // is bank #2 selected?


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
  pinMode(button_Custom, INPUT);
  pinMode(button_Bank, INPUT);

  // start USB HID interface
  Keyboard.begin();
}


/* ----------------------------------------------------------------------------------------------
   loop
   ----------------------------------------------------------------------------------------------
*/
void loop() {


  /* ----------------------------------------------------------------------------------------------
     TOGGLE SWITCH: BANK
     ----------------------------------------------------------------------------------------------
  */
  if (digitalRead(button_Bank) == LOW) {
    boolBankTwoSelected = false;
  } else {
    boolBankTwoSelected = true;
  }


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
      // do long-press action: nothing / Alt+E
      if (boolBankTwoSelected == false) {
        // nothing
      } else {
        pressKeys(false, false, true, 'e');
      }
    }
  }

  // button not pressed anymore
  else if (active_Speed == true) {
    active_Speed = false;

    // this was a short press
    if (active_Speed_LP == false) {
      // do short-press action: Shift+Alt+E / Ctrl+E
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 'e');
      } else {
        pressKeys(true, false, false, 'e');
      }
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
      // do long-press action: nothing / Alt+W
      if (boolBankTwoSelected == false) {
        // nothing
      } else {
        pressKeys(false, false, true, 'w');
      }
    }
  }

  // button not pressed anymore
  else if (active_Loop == true) {
    active_Loop = false;

    // this was a short press
    if (active_Loop_LP == false) {
      // do short-press action: Shift+Alt+W / Ctrl+W
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 'w');
      } else {
        pressKeys(true, false, false, 'w');
      }
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
      // do long-press action: Shift+Alt+R / Alt+A
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 'r');
      } else {
        pressKeys(false, false, true, 'r');
      }
    }
  }

  // button not pressed anymore
  else if (active_Back == true) {
    active_Back = false;

    // this was a short press
    if (active_Back_LP == false) {
      // do short-press action: Shift+Alt+A / Ctrl+A
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 'a');
      } else {
        pressKeys(true, false, false, 'a');
      }
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
      // do long-press action: Shift+Alt+Q / Alt+S
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 'q');
      } else {
        pressKeys(false, false, true, 's');
      }
    }
  }

  // button not pressed anymore
  else if (active_PlayPause == true) {
    active_PlayPause = false;

    // this was a short press
    if (active_PlayPause_LP == false) {
      // do short-press action: Shift+Alt+S / Ctrl+S
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 's');
      } else {
        pressKeys(true, false, false, 's');
      }
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
      // do long-press action: nothing / Alt+D
      if (boolBankTwoSelected == false) {
        // nothing
      } else {
        pressKeys(false, false, true, 'd');
      }
    }
  }

  // button not pressed anymore
  else if (active_Forward == true) {
    active_Forward = false;

    // this was a short press
    if (active_Forward_LP == false) {
      // do short-press action: Shift+Alt+D / Ctrl+D
      if (boolBankTwoSelected == false) {
        pressKeys(false, true, true, 'd');
      } else {
        pressKeys(true, false, false, 'd');
      }
    }

    // this was a long press
    else {
      active_Forward_LP = false;
    }
  }


  /* ----------------------------------------------------------------------------------------------
     BUTTON: CUSTOM
     ----------------------------------------------------------------------------------------------
  */

  // button pressed
  if (digitalRead(button_Custom) == HIGH) {

    // has not been pressed before => start timer
    if (active_Custom == false) {
      active_Custom = true;
      timer_Custom = millis();
    }

    // this is a long press
    if ((millis() - timer_Custom > longPressTime) && (active_Custom_LP == false)) {
      active_Custom_LP = true;
      // do long-press action: nothing / Alt+F
      if (boolBankTwoSelected == false) {
        // nothing
      } else {
        pressKeys(false, false, true, 'f');
      }
    }
  }

  // button not pressed anymore
  else if (active_Custom == true) {
    active_Custom = false;

    // this was a short press
    if (active_Custom_LP == false) {
      // do short-press action: nothing / Ctrl+F
      if (boolBankTwoSelected == false) {
        // nothing
      } else {
        pressKeys(true, false, false, 'f');
      }
    }

    // this was a long press
    else {
      active_Custom_LP = false;
    }
  }

}
