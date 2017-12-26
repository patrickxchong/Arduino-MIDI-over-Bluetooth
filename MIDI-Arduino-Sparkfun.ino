/*
Arduino MIDI project with Sparkfun MIDI Shield
MIDI-Arduino-Sparkfun.ino
Keyboard used: Yamaha YPT-220
Digital Audio Workstations tested on: Mixcraft 7, Ableton Live 9 (Reset All Controllers message does not work in Ableton Live 9)
Written by: Patrick Chong Jin Hua

Notes:
1. One MIDI message is composed of a set of three "Serial.write"s
The first is a command byte (type of MIDI message, eg. Note On)
The second and third are data bytes (eg. note value, velocity)
2. MIDI values are presented in decimals. eg. Note On = 144 (decimal) instead of 0x90 (hexadecimal)
*/

#include <QueueList.h> // For queue data structure. Source: https://playground.arduino.cc/Code/QueueList
#include <SoftwareSerial.h> // To read in MIDI data on pins 8 and 9
#include <MIDI.h> // Forty Seven Effects MIDI Library: To read in MIDI data
// have yet to find a way to read in MIDI data with the Software Serial without using this library

// Variables to hold MIDI input from keyboard
QueueList <byte> queue;
byte input;

// Variables to hold pitch bend and velocity information 
double vel = 0, pitch = 0, pitch_prev = 0;
bool pitch_bool = false, vel_bool = false;

// Declare and instantiate software serial to read MIDI input
SoftwareSerial MIDI_in(8, 9);
MIDI_CREATE_INSTANCE(SoftwareSerial, MIDI_in, MIDI);

void setup()
{
  // Start serial
  Serial.begin(9600);
  Serial.println("Setting up");

  // Set pinModes. Buttons: 2, 3, 4. LEDs: 6, 7 
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6, HIGH); 
  digitalWrite(7, HIGH); 

  // Receive messages on all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
  // BUTTON SETUPS
  // When the right button is pressed (variable velocity button)
  if (digitalRead(2) == LOW) {
    vel_bool=true;
    digitalWrite(6, LOW); // Green LED lights up
  }

  // When the left button is pressed (pitch bend button)
  if (digitalRead(4) == LOW) {
    pitch_bool=true;
    digitalWrite(7, LOW); // Red LED lights up
  }

  // When the middle button is pressed (reset variable velocity and pitch bend button)
  if (digitalRead(3) == LOW) {
    pitch_bool=false;
    digitalWrite(6, HIGH); 
    vel_bool=false;
    digitalWrite(7, HIGH); 

    // Send a Reset All Controllers message
    // When Reset All Controllers is received, all controller values are reset to their default values.
    // Controllers: Pitch bend, pedal etc
    // Note: this message doesn't work in Ableton Live 9 Suite. 
    // So be careful where the pitch bend (A0) toggle lands before switching off pitch bend
    Serial.write(176);
    Serial.write(121);
    Serial.write(0);
  }
  // END BUTTON SETUPS

  // READING IN MIDI SIGNALS
  // If there are MIDI Signals coming in, read in the information to a data byte 
  if(MIDI_in.available()){
    input = MIDI_in.read();

    // Only allows data bytes and pedal command bytes to go into the queue.
    if (input < 0x80 || input == 0xB0 || input == 0xB1 || input == 0xB2 ) queue.push(input);
    // All other command bytes are filtered out because in the keyboards that I tested, 
    // certain command bytes (0x80, 0xFE etc) are sent although they were not requested, 
    // and that messes up the MIDI signals that are sent to the serial port.
    // No other command bytes besides the pedal were added to the queue because 
    // the keyboard I used doesn't support the commands
  }
  // END READING IN MIDI SIGNALS

  // If the queue is not empty
  if (queue.count()!= 0) {    

    // SENDING PEDAL ON/OFF SIGNALS
    // Do nothing if the front of the queue is 176 (pedal command byte) 
    // but the number of items in the queue is not 9 (number of bytes sent by a pedal press)
    if ( (queue.peek() == 176)  && queue.count()!= 9) {
    }
    // Bulk send 9 bytes (pedal control change signal) if the front of the queue is 176 (pedal command byte) 
    // but the number of items in the queue is 9 (number of bytes sent by a pedal press)
    else if (queue.peek() == 176 && queue.count() == 9) {
      Serial.write(queue.pop()); //Pedal Command Byte + Channel 1
      Serial.write(queue.pop());
      Serial.write(queue.pop());
      Serial.write(queue.pop()); //Pedal Command Byte + Channel 2
      Serial.write(queue.pop());
      Serial.write(queue.pop());
      Serial.write(queue.pop()); //Pedal Command Byte + Channel 3
      Serial.write(queue.pop());
      Serial.write(queue.pop());
    }
    // END SENDING PEDAL ON/OFF SIGNALS


    else {

      // SENDING NOTE ON/OFF SIGNALS
      // Since the Note On signal is not in the queue, 
      // queue.pop is used only twice to remove note and velocity data bytes
      // If statement checks if the number of elements in the queue is an even number 
      // (since multiple bytes may be stored if multiple notes are played)
      if (queue.count()%2 == 0) {
        Serial.write(144); // Since the Note On byte is filtered out initially, it has to be manually created with 144 here.
        Serial.write(queue.pop()); //Note byte
        
        // VARIABLE VELOCITY BUTTON
        // NOTE: The variable velocity button is only included because my keyboard's velocity is not touch sensitive
        // but I wanted to be able to manipulate the velocity of the notes played. 

        // If variable velocity button (right button) is pressed and the velocity bit coming in is not zero,
        // get the velocity value from the potentiometer and sends that as the velocity byte
        if (vel_bool == true && queue.peek() != 0) {
          vel = analogRead(A1); // reads in analog value of right potentiometer
          Serial.write(int(vel/1023*127)); // potentiometer range (0-2013), velocity range (0-127)
          queue.pop(); // discard the original velocity byte
        } 
        // END VARIABLE VELOCITY BUTTON

        // Otherwise read the velocity byte directly from the queue
        else Serial.write(queue.pop()); 

      } 
      // END SENDING NOTE ON/OFF SIGNALS

    } 
    // end "if the queue is not empty"

  }
  
  // PITCH BEND BUTTON
  // NOTE: The pitch bend button is only included because my keyboard doesn't have a pitch bend toggle
  // but I wanted to be able to manipulate the pitch of the notes played. 

  // If pitch bend button (left button) is pressed, get the pitch bend value from the potentiometer 
  // and sends a pitch bend signal
  if (pitch_bool == true) {
    pitch = analogRead(A0); // reads in analog value of left potentiometer

    // Get the absolute difference of the pitch bend and previous pitch bend value 
    // Helps to reduce the number of pitch bend commands sent via MIDI
    // NOTE: Because analog signals can vary pitch != pitch_prev doesn't work very well here
    if (abs(pitch - pitch_prev) > 5) { 
      // If the analog value is below/above the threshold values, 
      // send a pitch bend command according to how much the potentiometer is turned
      if (pitch < 442 || pitch > 572) { 
        Serial.write(224);
        Serial.write(0);
        Serial.write(int(pitch/1023*127)); 
      }

      // Otherwise, keep notes at normal pitch.
      else { 
        Serial.write(224);
        Serial.write(0);
        Serial.write(64);
      }
      pitch_prev = pitch;
    }
  } 
  // END PITCH BEND BUTTON
}
