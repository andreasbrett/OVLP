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
byte midiChannel = 16;                // MIDI channel to use


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
    byte midiType = 255;          // type of MIDI message (1 = Note / 2 = CC / 3 = PC)
    byte midiNumber = 255;        // MIDI number to send
    byte midiValue = 255;         // MIDI value to send
    char character = ' ';         // character to press
  
    byte delay1 = 5;              // delay in msec before pressing character
    byte delay2 = 80;             // delay in msec before releasing character
    byte delay3 = 40;             // delay in msec before releasing modifiers
    byte delay4 = 100;            // delay in msec after sending a shortcut

  
  public:
    /* ----------------------------------------------------------------------------------------------
     * constructor
     * ----------------------------------------------------------------------------------------------
     */
    void init(String IN_shortcut) {
      shortcut = IN_shortcut;
      String s = "";
      char pos1 = 0;
      char pos2 = -1;
      
      // ---------------------------------------------------------
      // MIDI message (Note / CC / PC)
      // ---------------------------------------------------------
      if (shortcut.startsWith("MIDI")) {
        pos2 = shortcut.indexOf("#");
        s = shortcut.substring(pos1, pos2);
        s.toLowerCase();
        
        // MIDI-PC
        if (s == "midi-pc") {
          midiType = 3;
          pos1 = pos2+1;
          pos2 = shortcut.indexOf("#", pos1);
          s = shortcut.substring(pos1, pos2);
          midiNumber = s.toInt();
        } else {
          // MIDI-Note
          if (s == "midi-note") {
            midiType = 1;
          }
          
          // MIDI-CC
          else if (s == "midi-cc") {
            midiType = 2;
          }
          
          // fetch note/command
          pos1 = pos2+1;
          pos2 = shortcut.indexOf("#", pos1);
          s = shortcut.substring(pos1, pos2);
          midiNumber = s.toInt();
    
          // fetch note/command value
          pos1 = pos2+1;
          pos2 = shortcut.indexOf("#", pos1);
          s = shortcut.substring(pos1, pos2);
          midiValue = s.toInt();
        }
      }

      // ---------------------------------------------------------
      // keyboard shortcut
      // ---------------------------------------------------------
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
      // ---------------------------------------------------------
      // MIDI message (Note / CC / PC)
      // ---------------------------------------------------------
      if (midiType < 255) {
        
        // MIDI Note
        if (midiType == 1) {
          MIDI.sendNoteOn(midiNumber, midiValue, midiChannel);
          delay(delay2);
          MIDI.sendNoteOff(midiNumber, 0, midiChannel);
        }
        
        // MIDI Control Change
        else if (midiType == 2) {
          MIDI.sendControlChange(midiNumber, midiValue, midiChannel);
          delay(delay2);
          MIDI.sendControlChange(midiNumber, 0, midiChannel);
        }
        
        // MIDI Program Change
        else if (midiType == 3) {
          MIDI.sendProgramChange(midiNumber, midiChannel);  
        }
        
        delay(delay4);
      }

      // ---------------------------------------------------------
      // keyboard shortcut
      // ---------------------------------------------------------
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
    byte pin_button;                // button's assigned pin
    byte pin_bank;                  // bank switch' assigned pin
    byte currentState = 0;          // button's current state
    byte previousState = 0;         // button's previous state
    byte startPressed = 0;          // point of time button has been pressed
    byte endPressed = 0;            // point of time button has been released
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
   * examples for MIDI Note commands 
   * PARAM 1) note number: see https://newt.phys.unsw.edu.au/jw/notes.html
   * PARAM 2) velocity: 0-127
   *  - "MIDI-Note#0#127"
   *  - "MIDI-Note#23#100"
   *  - "MIDI-Note#127#43"
   *  
   * examples for MIDI Control Change (CC) commands
   * PARAM 1) control number: 0-127 
   * PARAM 2) control value: 0-127
   *  - "MIDI-CC#0#127"
   *  - "MIDI-CC#55#54"
   *  - "MIDI-CC#127#3"
   *  
   * examples for MIDI Program Change (PC) commands
   * PARAM 1) program number: 0-127
   *  - "MIDI-PC#0"
   *  - "MIDI-PC#55"
   *  - "MIDI-PC#127"
   */
  logMessage("Initializing buttons...");
  btnPlayPause.init(5, 16, "Shift#Alt#S", "Shift#Alt#Q", "MIDI-PC#5", "MIDI-PC#55");
  btnLoop.init(6, 16, "Shift#Alt#W", "Shift#Alt#T", "MIDI-PC#2", "MIDI-PC#22");
  btnBack.init(7, 16, "Shift#Alt#A", "Shift#Alt#R", "MIDI-PC#4", "MIDI-PC#44");
  btnFullscreen.init(8, 16, "Shift#Alt#F", "Shift#Alt#Z", "MIDI-PC#3", "MIDI-PC#33");
  btnForward.init(9, 16, "Shift#Alt#D", "Shift#Alt#H", "MIDI-PC#6", "MIDI-PC#66");
  btnSpeed.init(10, 16, "Shift#Alt#E", "Shift#Alt#G", "MIDI-PC#1", "MIDI-PC#11");

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
