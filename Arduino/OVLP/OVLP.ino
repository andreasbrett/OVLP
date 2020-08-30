/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * OVLP: Online Video Looper Pedal
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * AUTHOR  Andreas Brett
 * URL    https://github.com/andreasbrett/OVLP
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  - for tweaking
 *      - the pins that you used on your Arduino Micro
 *      - the keyboard shortcuts the pedal will send via USB
 *      - go to section "define buttons (pins and shortcuts)"
 *  - for tweaking the timings for a short vs long press go to section "global variables"
 *  - please consider not modifying the rest of this script unless you know what you're doing
 * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */




/* ----------------------------------------------------------------------------------------------
 * include keyboard and MIDI library
 * ----------------------------------------------------------------------------------------------
 */
#include <Keyboard.h>
#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();


/* ----------------------------------------------------------------------------------------------
 * global variables
 * ----------------------------------------------------------------------------------------------
 */
unsigned long shortPressTime = 30;    // how long will a switch need to be pressed to be considered a short press? [in milliseconds]
unsigned long longPressTime = 450;    // how long will a switch need to be pressed to be considered a long press? [in milliseconds]
bool logMessages = false;             // show log messages via serial connection?
int midiChannel = 16;                 // MIDI channel to use


/* ----------------------------------------------------------------------------------------------
 * log message via serial connection (for debugging)
 * ----------------------------------------------------------------------------------------------
 */
void logMessage(String s, bool bEOL = true) {
  if (logMessages == true) {
    Serial.print(s);
    if (bEOL == true) {
      Serial.println("");
    }
  }
}


/* ----------------------------------------------------------------------------------------------
 * CLASS: KbdShortcut
 * ----------------------------------------------------------------------------------------------
 */
class KbdShortcut {
  private:
    String shortcut = "<undef>";  // initial shortcut
    bool ctrlKey = false;         // press Ctrl key?
    bool shiftKey = false;        // press Shift key?
    bool altKey = false;          // press Alt key?
    int midiPC = -1;              // MIDI command
    char character = ' ';         // character to press
  
    int delay1 = 5;               // delay before pressing character
    int delay2 = 80;              // delay before releasing character
    int delay3 = 40;              // delay before releasing modifiers
    int delay4 = 100;             // delay after sending a shortcut

  
  public:
    /* ----------------------------------------------------------------------------------------------
     * constructor
     * ----------------------------------------------------------------------------------------------
     */
    void init(String IN_shortcut) {
      shortcut = IN_shortcut;
      String s = "";
      int pos1 = 0;
      int pos2 = -1;
      
      // --------------------------------
      // MIDI Program Change (PC) message
      // --------------------------------
      if (shortcut.startsWith("MIDI")) {
        pos1 = shortcut.indexOf("#", 0) + 1;
        pos2 = shortcut.length();
        s = shortcut.substring(pos1, pos2);
        midiPC = s.toInt();
      }

      // -----------------
      // keyboard shortcut
      // -----------------
      else {

        // go through each key in shortcut (delimiter: "#")
        do {
          pos2 = shortcut.indexOf("#", pos1);
          if (pos2 == -1) {
            pos2 = shortcut.length();
          }
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
        } while(pos1 <= shortcut.length());
    
        // remove delay1 and delay3 if no modifiers are set
        if (ctrlKey == false && shiftKey == false && altKey == false) {
          delay1 = 0;
          delay3 = 0;
        }
      }
    }

    /* ----------------------------------------------------------------------------------------------
     * return string representation of keyboard shortcut
     * ----------------------------------------------------------------------------------------------
     */
    String getShortcut() {
      return shortcut;
    }

  
    /* ----------------------------------------------------------------------------------------------
     * send keyboard shortcut
     * ----------------------------------------------------------------------------------------------
     */
    void send() {
      // --------------------------------
      // MIDI Program Change (PC) message
      // --------------------------------
      if (midiPC != -1) {
        MIDI.sendProgramChange(midiPC, midiChannel);
      }

      // -----------------
      // keyboard shortcut
      // -----------------
      else {
      
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
          delay(delay4);
        }
      }
    }
};


/* ----------------------------------------------------------------------------------------------
 * CLASS: Button
 * ----------------------------------------------------------------------------------------------
 */
class Button {
  private:
    int pin_button;                 // button's assigned pin
    int pin_bank;                   // bank switch' assigned pin
    int currentState = 0;           // button's current state
    int previousState = 0;          // button's previous state
    int startPressed = 0;           // point of time button has been pressed
    int endPressed = 0;             // point of time button has been released
    unsigned long holdTime = 0;     // time button has been pressed for
    KbdShortcut shortcut1_short;    // keyboard shortcut for short press (bank #1)
    KbdShortcut shortcut1_long;     // keyboard shortcut for long press (bank #1)
    KbdShortcut shortcut2_short;    // keyboard shortcut for short press (bank #2)
    KbdShortcut shortcut2_long;     // keyboard shortcut for long press (bank #2)

  public:

    /* ----------------------------------------------------------------------------------------------
     * constructor
     * ----------------------------------------------------------------------------------------------
     */
    void init(int IN_pin_button, int IN_pin_bank, String IN_short1, String IN_long1, String IN_short2, String IN_long2) {
      // set pin mode
      pinMode(IN_pin_button, INPUT);
      pinMode(IN_pin_bank, INPUT);

      // define shortcuts
      shortcut1_short.init(IN_short1);
      shortcut1_long.init(IN_long1);
      shortcut2_short.init(IN_short2);
      shortcut2_long.init(IN_long2);
      
      // store pins
      pin_button = IN_pin_button;
      pin_bank = IN_pin_bank;

      logMessage("Initialized button on pin ", false);
      logMessage(String(pin_button));
    }
  
    
    /* ----------------------------------------------------------------------------------------------
     * check if button changed state
     * ----------------------------------------------------------------------------------------------
     */
    void check() {
      currentState = digitalRead(pin_button);
      if (currentState != previousState) {
        react();
      }
      previousState = currentState;
    }
  
    
    /* ----------------------------------------------------------------------------------------------
     * react upon short/long press
     * ----------------------------------------------------------------------------------------------
     */
    void react() {
      // button pressed
      if (currentState == HIGH) {
          startPressed = millis();
    
      // button released
      } else {
          endPressed = millis();
          holdTime = endPressed - startPressed;
    
          // short-press
          if (holdTime >= shortPressTime && holdTime < longPressTime) {
              if (digitalRead(pin_bank) == LOW) {
                logMessage("Button short-pressed (Bank #1). Sending keystrokes: ", false);
                logMessage(shortcut1_short.getShortcut());
                shortcut1_short.send();
              } else {
                logMessage("Button short-pressed (Bank #2). Sending keystrokes: ", false);
                logMessage(shortcut2_short.getShortcut());  
                shortcut2_short.send();
              }
          }

          // long-press
          if (holdTime >= longPressTime) {
              if (digitalRead(pin_bank) == LOW) {
                logMessage("Button long-pressed (Bank #1). Sending keystrokes: ", false);
                logMessage(shortcut1_long.getShortcut());
                shortcut1_long.send();
              } else {
                logMessage("Button long-pressed (Bank #2). Sending keystrokes: ", false);
                logMessage(shortcut2_long.getShortcut());
                shortcut2_long.send();
              }
          }
      }
    }
};


// button objects
Button btnPlayPause;
Button btnLoop;
Button btnBack;
Button btnFullscreen;
Button btnForward;
Button btnSpeed;

/* ----------------------------------------------------------------------------------------------
 * setup
 * ----------------------------------------------------------------------------------------------
 */
void setup() {
  // initialize serial communication (wait for serial stream to open)
  if (logMessages == true) {
    Serial.begin(9600);
    while (!Serial) ;
    logMessage("---------------------------------------------");
    logMessage("Serial interface initialized.");
  }

  // start USB HID interface
  logMessage("Initializing USB HID interface...");
  Keyboard.begin();

  // start MIDI interface
  logMessage("Initializing USB MIDI interface...");
  MIDI.begin(midiChannel);

  /* ----------------------------------------------------------------------------------------------
   * define buttons (pins and shortcuts)
   * ----------------------------------------------------------------------------------------------
   * PARAM 1) Arduino pin that is mapped to the button
   * PARAM 2) Arduino pin that is mapped to the bank switch
   * PARAM 3) bank #1: shortcut to send when short-pressed
   * PARAM 4) bank #1: shortcut to send when long-pressed
   * PARAM 5) bank #2: shortcut to send when short-pressed
   * PARAM 6) bank #2: shortcut to send when long-pressed
   * 
   * ----------------------------------------------------------------------------------------------
   * shortcut format => concatenate MODIFIER KEYS (ctrl, shift, alt) and NORMAL KEY using "#"
   * ----------------------------------------------------------------------------------------------
   *
   * examples:
   *  - "Shift#Alt#E"
   *  - "Ctrl#K"
   *  - "Alt#X"
   *
   * for SPECIAL KEYS (e.g. page up/down, up/down/left/right, escape, enter, and so on) check:
   * https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/
   *
   * examples:
   *  - "Shift#Alt#" + KEY_F4
   *  - "Shift#Alt#" + KEY_TAB
   *  - "Shift#Alt#" + KEY_DOWN_ARROW
   *  
   * examples for MIDI Program Change (PC) commands:
   *  - "MIDI#0"
   *  - "MIDI#55"
   *  - "MIDI#127"
   */
  logMessage("Initializing buttons...");
  btnPlayPause.init(5, 16, "Shift#Alt#S", "Shift#Alt#Q", "MIDI#5", "MIDI#55");
  btnLoop.init(6, 16, "Shift#Alt#W", "Shift#Alt#T", "MIDI#2", "MIDI#22");
  btnBack.init(7, 16, "Shift#Alt#A", "Shift#Alt#R", "MIDI#4", "MIDI#44");
  btnFullscreen.init(8, 16, "Shift#Alt#F", "Shift#Alt#Z", "MIDI#3", "MIDI#33");
  btnForward.init(9, 16, "Shift#Alt#D", "Shift#Alt#H", "MIDI#6", "MIDI#66");
  btnSpeed.init(10, 16, "Shift#Alt#E", "Shift#Alt#G", "MIDI#1", "MIDI#11");

  logMessage("Initialization done. Running loop now.");
  logMessage("---------------------------------------------");
}

/* ----------------------------------------------------------------------------------------------
 * loop
 * ----------------------------------------------------------------------------------------------
 */
void loop() {
  // check each button for short and long presses
  btnPlayPause.check();
  btnLoop.check();
  btnBack.check();
  btnFullscreen.check();
  btnForward.check();
  btnSpeed.check();
}
