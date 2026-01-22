/*
 * Based on https://wiki.mythtv.org/wiki/Arduino_Infrared_Remote_Control 
 * Arduino_Infrared_Remote_Control.ino
 *
 * Code to receive an RC6 infrared code and then send a corresponding keyboard character via USB.
 * Uses the Arduino IRremote example dump code to determine the address and command codes for your
 * specific remote control. You can remove comments below to scan codes from your remote.
 * Note that Keyboard emulation will not work when Serial.print is enabled.
 * To receive infrared codes, connect a 38 kHz infrared receiver (Example TSOP 38038).
 * Check the datasheet for your IR receiver to determine which pin connects to Pin 2 and which
 * pins connect to 3.3 volts and ground.
 * You must use an Arduino model with HID support in order to emulate a keyboard.
 * Example: Arduino Uno R4 Minima.
 *
 * This was tested with a Teensy 3.2 and a random IR reciever salvaged from some junk.
 * MIT License
 *
 * Copyright (c) 2025 Dennis McCloud
 * Copyright (c) 2026 Shawn D'Alimonte
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Acknowledgement to Ken Shirriff, Rafi Khan and Armin Joachimsmeyer et al. who developed
 * the Arduino-IRremote code and library which was the source and inspiration
 * for this code and Tom Igoe who developed the Arduino Keyboard code and library.
 */
#define IR_SEND_PIN 3     // Dummy -  Teensyduino complains if it isn't set
#define IR_RECEIVE_PIN 2  // IR Receive Pin on Arduino UNO R4 Minima
#define DECODE_RC6        // Decode RC6 - Change for other remotes

#include <Arduino.h>
#include <IRremote.hpp>  // include the library
#include "Keyboard.h"

// Uncomment this to show codes on the serial i/f for finding key codes
//#define SHOW_CODES

void setup() {

#ifdef SHOW_CODES
  Serial.begin(115200);
#endif

  // Start the Infrared receiver
  IrReceiver.begin(IR_RECEIVE_PIN);

  // Initialize the built-in LED to flash when sending characters
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef SHOW_CODES
  Serial.println("IR Remote Tester");
#endif

  // Initialize control over the keyboard:
  // Note that you must disable the serial port when using the HID keyboard simulation
  Keyboard.begin();
}

void loop() {

  /*
 * Check if received data is available and decode it.
 * Decoded result is in the IrReceiver.decodedIRData structure.
 */

  if (IrReceiver.decode()) {

    // Turn On Built-In LED
    digitalWrite(LED_BUILTIN, HIGH);

    /*
 * Received an unknown protocol, skip that data - probably noise
*/
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      IrReceiver.resume();  // Enable receiving of the next IR code
      return;
    } else {
      IrReceiver.resume();  // Enable receiving of the next IR code
    }
    // Ignore Duplicate Received IR Signals
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();  // Enable receiving of the next IR code
      return;
    }
    /*
 * If IR signal received was not RC6 then ignore
 * Change to match your specific remote control
 */
#ifdef SHOW_CODES
    Serial.print(IrReceiver.decodedIRData.protocol);
    Serial.print(" / ");
    Serial.print(IrReceiver.decodedIRData.address);
    Serial.print(" / ");
    Serial.println(IrReceiver.decodedIRData.command);
    IrReceiver.resume();return; // Skip sending keys for now
#endif

    // All keys on this remote use Address 0x04
    if (IrReceiver.decodedIRData.address != 0x04) {
      IrReceiver.resume();  // Enable receiving of the next IR code
      return;
    }

    // For reference, below are the key definitions for Keyboard.h routine:
    // KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT also RIGHT versions
    // KEY_BACKSPACE, KEY_TAB, KEY_RETURN (Enter), and KEY_ESC (Escape)
    // KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW
    // KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, and KEY_END
    // KEY_KEYPAD_0 to KEY_KEYPAD_9, KEY_KEYPAD_ENTER, KEY_KEYPAD_PLUS
    // KEY_KEYPAD_MINUS, KEY_KEYPAD_MULTIPLY, KEY_KEYPAD_DIVIDE,
    // and KEY_KEYPAD_NUMLOCK
    //
    // Below are the different formats for the Keyboard.press, press, and release/All routines:
    // Send a keyboard key emulation, P for pause:
    //    Keyboard.press('P');
    // Press and Hold a key
    //        Keyboard.press(<keyCode>);
    // Release a held key
    //        Keyboard.release(<keyCode>);
    // Release all held keys
    //        Keyboard.releaseAll();

    // Below are the codes that are used for MythTV remote control
    // These are for the Hauppauge black remote control which is RC6 IR protocol

    // Change IR codes to match your specific remote control
    // PLAY or OK Buttons on remote translate to Return

    // Key bindings based on https://wiki.mythtv.org/wiki/Keybindings#mythfrontend
    switch (IrReceiver.decodedIRData.command) {
      case 0x00: Keyboard.press('0'); break;              // 0
      case 0x01: Keyboard.press('1'); break;              // 1
      case 0x02: Keyboard.press('2'); break;              // 2
      case 0x03: Keyboard.press('3'); break;              // 3
      case 0x04: Keyboard.press('4'); break;              // 4
      case 0x05: Keyboard.press('5'); break;              // 5
      case 0x06: Keyboard.press('6'); break;              // 6
      case 0x07: Keyboard.press('7'); break;              // 7
      case 0x08: Keyboard.press('8'); break;              // 8
      case 0x09: Keyboard.press('9'); break;              // 9
      case 0x0A: Keyboard.press('d'); break;              // Clear - Used as delete
      case 0x0B: Keyboard.press(KEY_RETURN); break;       // Enter
      case 0x0C: break;                                   // Power
      case 0x0D: Keyboard.press('m'); break;              // Windows Logo as Menu
      case 0x0E: Keyboard.press(KEY_MEDIA_MUTE); break;   // Mute
      case 0x0F: Keyboard.press('i'); break;              // Info
      case 0x10: Keyboard.press(KEY_MEDIA_VOLUME_INC); break;    // Volume Up
      case 0x11: Keyboard.press(KEY_MEDIA_VOLUME_DEC); break;  // Volume Down
      case 0x12: Keyboard.press(KEY_UP_ARROW); break;     // Channel Up
      case 0x13: Keyboard.press(KEY_DOWN_ARROW); break;   // Channel Down
      case 0x14: Keyboard.press(KEY_PAGE_DOWN); break;    // Fast Forward
      case 0x15: Keyboard.press(KEY_PAGE_UP); break;      // Rewind
      case 0x16: Keyboard.press(KEY_RETURN); break;       // Play
      case 0x17: Keyboard.press(KEY_RETURN); break;       // Record
      case 0x18: Keyboard.press('p'); break;              // Pause
      case 0x19: break;                                   // Stop
      case 0x1A: Keyboard.press('Z'); break;              // Next
      case 0x1B: Keyboard.press('Q'); break;              // Previous
      case 0x1E: Keyboard.press(KEY_UP_ARROW); break;     // Up
      case 0x1F: Keyboard.press(KEY_DOWN_ARROW); break;   // Down
      case 0x20: Keyboard.press(KEY_LEFT_ARROW); break;   // Left
      case 0x21: Keyboard.press(KEY_RIGHT_ARROW); break;  // Right
      case 0x22: Keyboard.press(KEY_RETURN); break;       // OK
      case 0x23: Keyboard.press(KEY_ESC); break;          // Back
      case 0x24: break;                                   // Play Disc
      case 0x25: break;                                   // Play PVR
      case 0x26: Keyboard.press('s'); break;              // Guide
      case 0x48: break;                                   // Record PVR
      case 0x5A: Keyboard.press('w'); break;              // T - Mapped as Zoom
      case 0x5B: Keyboard.press('d'); break;              // Red - Mapped as Delete
      case 0x5C: break;                                   // Green
      case 0x5D: break;                                   // Yellow
      case 0x5E: Keyboard.press('A'); break;              // Blue - mapped as slow
      default: break;                                     // Unknown button
    }
    delay(100);
    Keyboard.releaseAll();
  }
  // Turn off Built-In LED
  digitalWrite(LED_BUILTIN, LOW);
}