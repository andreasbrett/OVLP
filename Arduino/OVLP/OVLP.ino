/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * OVLP: Online Video Looper Pedal
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * AUTHOR  Andreas Brett
 * URL     https://github.com/andreasbrett/OVLP
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  - for tweaking the pins that you used on your Arduino Micro go to section "define pin <-> switch mappings"
 *  - for tweaking the shortscuts that the pedal will send via USB go to section "define shortcuts to send when switch is pressed"
 *  - for tweaking the timings for a long press go to section "global variables"
 *  - please consider not modifying the rest of this script unless you know what you're doing
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */




/* ----------------------------------------------------------------------------------------------
 * include keyboard library
 * ----------------------------------------------------------------------------------------------
 */
#include "Keyboard.h"


/* ----------------------------------------------------------------------------------------------
 * CLASS: KbdShortcut
 * ----------------------------------------------------------------------------------------------
 */
class KbdShortcut {
  public:
    bool ctrlKey = false;   // press Ctrl key?
    bool shiftKey = false;    // press Shift key?
    bool altKey = false;    // press Alt key?
    char character = ' ';   // character to press

    int delay1 = 0;       // delay before pressing character
    int delay2 = 80;      // delay before releasing character
    int delay3 = 40;      // delay before releasing modifiers


  /* ----------------------------------------------------------------------------------------------
   * constructor
   * ----------------------------------------------------------------------------------------------
   */
    KbdShortcut(String shortcut) {
    String s = "";
    int pos1 = 0;
    int pos2 = -1;

    // go through each key in shortcut (delimiter: "+")
    do {
    pos2 = shortcut.indexOf("+", pos1);
    if (pos2 != -1) {
      s = shortcut.substring(pos1, pos2);
      s.toLowerCase();
      if (s == "ctrl") {
       ctrlKey = true;
      } else if (s == "shift") {
       shiftKey = true;
      } else if (s == "alt") {
       altKey = true;
      } else {
      character = s.charAt(0);
      }
      pos1 = pos2+1;
    }
    } while(pos2 != -1);

    // remove delay1 and delay3 if no modifiers are set
    if (ctrlKey == false && shiftKey == false && altKey == false) {
    delay1 = 0;
    delay3 = 0;
    }

    }


  /* ----------------------------------------------------------------------------------------------
   * send keyboard shortcut
   * ----------------------------------------------------------------------------------------------
   */
    void send() {
    // check if character is set
    if (character != ' ') {
    // press Ctrl if necessary
    if (ctrlKey) {
      Keyboard.press(KEY_LEFT_CTRL);
    }

    // press Shift if necessary
    if (shiftKey) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }

    // press Alt if necessary
    if (altKey) {
      Keyboard.press(KEY_LEFT_ALT);
    }

    // press character
    delay(delay1);
    Keyboard.press(character);

    // release character
    delay(delay2);
    Keyboard.release(character);

    // release modifiers
    delay(delay3);
    Keyboard.releaseAll();
    }
    }
};

/* ----------------------------------------------------------------------------------------------
 * define pin <-> switch mappings
 * ----------------------------------------------------------------------------------------------
 */
#define switch_Speed 5
#define switch_Loop 6
#define switch_Back 7
#define switch_PlayPause 8
#define switch_Forward 9
#define switch_Custom 10
#define switch_Bank 11


/* ----------------------------------------------------------------------------------------------
 * define shortcuts to send when switch is pressed (bank 1)
 * ----------------------------------------------------------------------------------------------
 */
KbdShortcut shortcut_Speed("Shift+Alt+E");
KbdShortcut shortcut_Speed_Long("");
KbdShortcut shortcut_Loop("Shift+Alt+W");
KbdShortcut shortcut_Loop_Long("");
KbdShortcut shortcut_Back("Shift+Alt+A");
KbdShortcut shortcut_Back_Long("Shift+Alt+R");
KbdShortcut shortcut_PlayPause("Shift+Alt+S");
KbdShortcut shortcut_PlayPause_Long("Shift+Alt+Q");
KbdShortcut shortcut_Forward("Shift+Alt+D");
KbdShortcut shortcut_Forward_Long("");
KbdShortcut shortcut_Custom("");
KbdShortcut shortcut_Custom_Long("");


/* ----------------------------------------------------------------------------------------------
 * define shortcuts to send when switch is pressed (bank 2)
 * ----------------------------------------------------------------------------------------------
 */
KbdShortcut shortcut_Speed2("Ctrl+E");
KbdShortcut shortcut_Speed2_Long("Alt+E");
KbdShortcut shortcut_Loop2("Ctrl+W");
KbdShortcut shortcut_Loop2_Long("Alt+W");
KbdShortcut shortcut_Back2("Ctrl+A");
KbdShortcut shortcut_Back2_Long("Alt+A");
KbdShortcut shortcut_PlayPause2("Ctrl+S");
KbdShortcut shortcut_PlayPause2_Long("Alt+S");
KbdShortcut shortcut_Forward2("Ctrl+D");
KbdShortcut shortcut_Forward2_Long("Alt+D");
KbdShortcut shortcut_Custom2("Ctrl+F");
KbdShortcut shortcut_Custom2_Long("Alt+F");


/* ----------------------------------------------------------------------------------------------
 * global variables
 * ----------------------------------------------------------------------------------------------
 */
unsigned long longPressTime = 1250; // how long will a switch need to be pressed to be considered a long press? [in milliseconds]

bool active_Speed = false;      // speed switch press active?
bool active_Speed_LP = false;   // speed switch long-press active?
unsigned long timer_Speed = 0;    // speed switch long-press timer

bool active_Loop = false;     // loop switch press active?
bool active_Loop_LP = false;    // loop switch long-press active?
unsigned long timer_Loop = 0;   // loop switch long-press timer

bool active_Back = false;     // back switch press active?
bool active_Back_LP = false;    // back switch long-press active?
unsigned long timer_Back = 0;   // back switch long-press timer

bool active_PlayPause = false;    // play/pause switch press active?
bool active_PlayPause_LP = false; // play/pause switch long-press active?
unsigned long timer_PlayPause = 0;  // play/pause switch long-press timer

bool active_Forward = false;    // forward switch press active?
bool active_Forward_LP = false;   // forward switch long-press active?
unsigned long timer_Forward = 0;  // forward switch long-press timer

bool active_Custom = false;     // custom switch press active?
bool active_Custom_LP = false;    // custom switch long-press active?
unsigned long timer_Custom = 0;   // custom switch long-press timer

bool boolBankTwoSelected = false; // is bank #2 selected?


/* ----------------------------------------------------------------------------------------------
 * setup
 * ----------------------------------------------------------------------------------------------
 */
void setup() {
  // setup pins as inputs
  pinMode(switch_Speed, INPUT);
  pinMode(switch_Loop, INPUT);
  pinMode(switch_Back, INPUT);
  pinMode(switch_PlayPause, INPUT);
  pinMode(switch_Forward, INPUT);
  pinMode(switch_Custom, INPUT);
  pinMode(switch_Bank, INPUT);

  // start USB HID interface
  Keyboard.begin();
}


/* ----------------------------------------------------------------------------------------------
 * loop
 * ----------------------------------------------------------------------------------------------
 */
void loop() {


  /* ----------------------------------------------------------------------------------------------
   * TOGGLE SWITCH: BANK
   * ----------------------------------------------------------------------------------------------
   */
  if (digitalRead(switch_Bank) == LOW) {
  boolBankTwoSelected = false;
  } else {
  boolBankTwoSelected = true;
  }


  /* ----------------------------------------------------------------------------------------------
   * MOMENTARY SWITCH: SPEED
   * ----------------------------------------------------------------------------------------------
   */

  // switch pressed
  if (digitalRead(switch_Speed) == HIGH) {

  // has not been pressed before => start timer
  if (active_Speed == false) {
    active_Speed = true;
    timer_Speed = millis();
  }

  // this is a long press
  if ((millis() - timer_Speed > longPressTime) && (active_Speed_LP == false)) {
    active_Speed_LP = true;
    // run long-press action
    if (boolBankTwoSelected == false) {
    shortcut_Speed_Long.send();
    } else {
    shortcut_Speed2_Long.send();
    }
  }
  }

  // switch not pressed anymore
  else if (active_Speed == true) {
  active_Speed = false;

  // this was a short press
  if (active_Speed_LP == false) {
    // run short-press action
    if (boolBankTwoSelected == false) {
    shortcut_Speed.send();
    } else {
    shortcut_Speed2.send();
    }
  }

  // this was a long press
  else {
    active_Speed_LP = false;
  }
  }


  /* ----------------------------------------------------------------------------------------------
   * MOMENTARY SWITCH: LOOP
   * ----------------------------------------------------------------------------------------------
   */

  // switch pressed
  if (digitalRead(switch_Loop) == HIGH) {

  // has not been pressed before => start timer
  if (active_Loop == false) {
    active_Loop = true;
    timer_Loop = millis();
  }

  // this is a long press
  if ((millis() - timer_Loop > longPressTime) && (active_Loop_LP == false)) {
    active_Loop_LP = true;
    // run long-press action
    if (boolBankTwoSelected == false) {
    shortcut_Loop_Long.send();
    } else {
    shortcut_Loop2_Long.send();
    }
  }
  }

  // switch not pressed anymore
  else if (active_Loop == true) {
  active_Loop = false;

  // this was a short press
  if (active_Loop_LP == false) {
    // run short-press action
    if (boolBankTwoSelected == false) {
    shortcut_Loop.send();
    } else {
    shortcut_Loop2.send();
    }
  }

  // this was a long press
  else {
    active_Loop_LP = false;
  }
  }


  /* ----------------------------------------------------------------------------------------------
   * MOMENTARY SWITCH: BACK
   * ----------------------------------------------------------------------------------------------
   */

  // switch pressed
  if (digitalRead(switch_Back) == HIGH) {

  // has not been pressed before => start timer
  if (active_Back == false) {
    active_Back = true;
    timer_Back = millis();
  }

  // this is a long press
  if ((millis() - timer_Back > longPressTime) && (active_Back_LP == false)) {
    active_Back_LP = true;
    // run long-press action
    if (boolBankTwoSelected == false) {
    shortcut_Back_Long.send();
    } else {
    shortcut_Back2_Long.send();
    }
  }
  }

  // switch not pressed anymore
  else if (active_Back == true) {
  active_Back = false;

  // this was a short press
  if (active_Back_LP == false) {
    // run short-press action
    if (boolBankTwoSelected == false) {
    shortcut_Back.send();
    } else {
    shortcut_Back2.send();
    }
  }

  // this was a long press
  else {
    active_Back_LP = false;
  }
  }


  /* ----------------------------------------------------------------------------------------------
   * MOMENTARY SWITCH: PLAY/PAUSE
   * ----------------------------------------------------------------------------------------------
   */

  // switch pressed
  if (digitalRead(switch_PlayPause) == HIGH) {

  // has not been pressed before => start timer
  if (active_PlayPause == false) {
    active_PlayPause = true;
    timer_PlayPause = millis();
  }

  // this is a long press
  if ((millis() - timer_PlayPause > longPressTime) && (active_PlayPause_LP == false)) {
    active_PlayPause_LP = true;
    // run long-press action
    if (boolBankTwoSelected == false) {
    shortcut_PlayPause_Long.send();
    } else {
    shortcut_PlayPause2_Long.send();
    }
  }
  }

  // switch not pressed anymore
  else if (active_PlayPause == true) {
  active_PlayPause = false;

  // this was a short press
  if (active_PlayPause_LP == false) {
    // run short-press action
    if (boolBankTwoSelected == false) {
    shortcut_PlayPause.send();
    } else {
    shortcut_PlayPause2.send();
    }
  }

  // this was a long press
  else {
    active_PlayPause_LP = false;
  }
  }


  /* ----------------------------------------------------------------------------------------------
   * MOMENTARY SWITCH: FORWARD
   * ----------------------------------------------------------------------------------------------
   */

  // switch pressed
  if (digitalRead(switch_Forward) == HIGH) {

  // has not been pressed before => start timer
  if (active_Forward == false) {
    active_Forward = true;
    timer_Forward = millis();
  }

  // this is a long press
  if ((millis() - timer_Forward > longPressTime) && (active_Forward_LP == false)) {
    active_Forward_LP = true;
    // run long-press action
    if (boolBankTwoSelected == false) {
    shortcut_Forward_Long.send();
    } else {
    shortcut_Forward2_Long.send();
    }
  }
  }

  // switch not pressed anymore
  else if (active_Forward == true) {
  active_Forward = false;

  // this was a short press
  if (active_Forward_LP == false) {
    // run short-press action
    if (boolBankTwoSelected == false) {
    shortcut_Forward.send();
    } else {
    shortcut_Forward2.send();
    }
  }

  // this was a long press
  else {
    active_Forward_LP = false;
  }
  }


  /* ----------------------------------------------------------------------------------------------
   * MOMENTARY SWITCH: CUSTOM
   * ----------------------------------------------------------------------------------------------
   */

  // switch pressed
  if (digitalRead(switch_Custom) == HIGH) {

  // has not been pressed before => start timer
  if (active_Custom == false) {
    active_Custom = true;
    timer_Custom = millis();
  }

  // this is a long press
  if ((millis() - timer_Custom > longPressTime) && (active_Custom_LP == false)) {
    active_Custom_LP = true;
    // run long-press action
    if (boolBankTwoSelected == false) {
    shortcut_Custom_Long.send();
    } else {
    shortcut_Custom2_Long.send();
    }
  }
  }

  // switch not pressed anymore
  else if (active_Custom == true) {
  active_Custom = false;

  // this was a short press
  if (active_Custom_LP == false) {
    // run short-press action
    if (boolBankTwoSelected == false) {
    shortcut_Custom.send();
    } else {
    shortcut_Custom2.send();
    }
  }

  // this was a long press
  else {
    active_Custom_LP = false;
  }
  }

}
