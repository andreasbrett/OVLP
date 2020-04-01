# Online Video Looper Pedal
This is a DIY clone of the [Vidami Pedal](https://utility.design) by Utility Design giving you handsfree looping and playback control on **Youtube** and **Vimeo** videos.

## Reviews of the Vidami Pedal
- [Review by R.J. Ronquillo](https://www.youtube.com/watch?v=VKHbrlyeI6I)
- [Review by Steve Stine](https://www.youtube.com/watch?v=Oeq5DZQMakY)

## Parts to get
- 1x [Arduino Pro Micro](https://www.amazon.de/dp/B01KMFRCN0/)
- 1x [1590XX](https://www.taydaelectronics.com/catalogsearch/result/?q=1590xx) or [1590DD](https://www.taydaelectronics.com/catalogsearch/result/?q=1590dd) enclosure
- 5x [Soft momentary footswitches](https://www.taydaelectronics.com/spst-momentary-soft-touch-push-button-stomp-foots-pedal-switch.html)
- 5x [10kOhm resistors](https://www.taydaelectronics.com/10k-ohm-1-2w-5-carbon-film-resistor.html)
- 1x [USB cable (USB-A to Micro-USB)](https://www.amazon.de/dp/B019Q6Y2MK/)
- some wire for soldering it all together

## Hardware Installation

### Arduino
 - check the pin layout of your Arduino (Pro) Micro
    - **NOTE**: the one linked above is different from the official one
    - [pinout for above linked Arduino Pro Micro](https://github.com/sparkfun/Pro_Micro/raw/master/Documentation/ProMicro16MHzv2.pdf)
    - [latest pinout for official Arduino Micro](https://content.arduino.cc/assets/Pinout-Micro_latest.png)
 - connect / solder wires to
    - 5V pin
    - GND Pin
    - 5 digital inputs
      - e.g. 5, 6, 7, 8, 9
      - if you choose custom ones you will have to modify the Arduino code
 - connect the other ends to your footswitches and resistors as shown in the following schematic:
 
 ![Schematic](https://github.com/andreasbrett/OVLP/raw/master/Schematic/schematic.png)

### Enclosure
 - drill 5 holes for the footswitches
 - drill a hole for the USB cable
 - mount the Arduino Micro so it won't move (e.g. a [small breadboard](https://www.taydaelectronics.com/catalogsearch/result/?q=170+point+breadboard))
 - mount the footswitches

## Software Installation
 - load and flash [OVLP.ino](https://github.com/andreasbrett/OVLP/raw/master/Arduino/OVLP/OVLP.ino) onto the Arduino using [Arduino IDE 1.6.7 or newer](https://www.arduino.cc/en/Main/Software)
 - install [Tampermonkey extension](https://www.tampermonkey.net/) in Chrome/Firefox/Edge/Opera/Safari
 - import [OVLP.user.js](https://github.com/andreasbrett/OVLP/raw/master/Tampermonkey/OVLP.user.js) into Tampermonkey

## Usage
 - pressing button **Speed** will toggle between playback rates (100% -> 75% -> 50% -> 35% -> 20% and back again to 100%)
 - pressing button **Loop**
    - 1st time: sets start of the loop
    - 2nd time: sets end of the loop
    - 3rd time: clears loop
 - buttons **Play / Pause**, **Back** and **Forward** are obviously playback controls
 - pressing **Play / Pause** button for more than 1sec will reset the playback rate and clear the loop
 - pressing **Back** button for more than 1sec will rewind the video
