# Arduino MIDI over Bluetooth using SparkFun MIDI Shield

A MIDI based Arduino project using SparkFun's MIDI Shield. I created my own method of parsing MIDI signals in the code because the MIDI Library by Forty Seven Effects was not accurate enough for my intended purposes - to send MIDI Signals from the keyboard to my laptop wirelessly over Bluetooth. Since Serial in Arduino can be both accessed via USB and Bluetooth, no modification is required to use this code via USB. In fact, this guide assumes that setup is done via USB, and the Bluetooth component is covered later as an extension.

I tried to make my code as well commented as possible to aid anyone working with microcontrollers and MIDI, but if anything is not clear, do open an issue and I'll try my best to make things clearer/clarify your queries! 

Diagram of completed MIDI over Bluetooth prototype
![MIDI-with-Arduino-and-SparkFun-MIDI-Shield](https://github.com/patch153/MIDI-with-Arduino-and-SparkFun-MIDI-Shield/blob/master/MBlue.jpg?raw=true)

## Getting Started/Prerequisites

### Hardware:
* Arduino UNO
* Sparkfun's MIDI Shield (https://www.sparkfun.com/products/12898) - best if set up using stackable headers especially if you want to send MIDI Signals to the laptop over bluetooth
* Jumper wires/cables
* MIDI cable(s)
* HC-06 Bluetooth module (can be easily found online)

### Software (Arduino Code):
* QueueList.h: https://playground.arduino.cc/Code/QueueList
* SoftwareSerial.h: Already part of Arduino's IDE
* MIDI.h: https://github.com/FortySevenEffects/arduino_midi_library

### Software (Laptop):
* Hairless MIDI Serial: http://projectgus.github.io/hairless-midiserial/ (converts Serial data into MIDI signals)
* Loop MIDI: https://www.tobias-erichsen.de/software/loopmidi.html (interface between Hairless and DAW)
* Any Digital Audio Workstation (DAW)

### Installing

* Set up hardware and install required software
* Download MIDI-Arduino-Sparkfun.ino and adapt it to your purposes. 
* Flash code to Arduino
* Enjoy!

## Extension: Bluetooth
As mentioned earlier, this project was meant to create a working prototype that sends MIDI signals from a piano keyboard to a laptop wirelessly over Bluetooth. No code modification is necessary besides installing a Bluetooth module and connecting it to the Arduino's TX and RX pins. The Bluetooth module used in my project was a HC-06. 

## Authors

* **Patrick Chong Jin Hua** - *Initial work and main contributer*

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments
* QueueList Library For Arduino by Efstathios Chatzikyriakidis (contact@efxa.org)
* SparkFun's MIDI Shield Hookup Guide
* Simon Alexander Adams, my ENTR390-008 lecturer, who's constant encouragement whenever I hit roadblocks allowed me to finally create a funtional and practical prototype that sends MIDI signals over Bluetooth to a DAW in my laptop
* Hunter from my ENTR390-008 section. Seeing him use a Stack for his project made me realize that I can use data structures to store the MIDI signals first before bulk releasing them in order to avoid incomplete MIDI signals

## Resources
* https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html
* https://learn.sparkfun.com/tutorials/midi-shield-hookup-guide
* https://www.midi.org/specifications/item/table-1-summary-of-midi-message

## Buy me a coffee (or bubble tea)
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/patrickxchong)
