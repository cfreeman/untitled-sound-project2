/*
 * Copyright (c) Clinton Freeman 2012
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <tables/cos8192_int8.h> // table for Oscils to play
#include <tables/sin8192_int8.h>
#include <Utils.h>

#define CONTROL_RATE 64 // powers of 2 please
#define POLL_RATE 10000 // milliseconds between polling data for logging.

int vegAPin = 0;
char vegAGain = 0;
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> vegABase(SIN8192_DATA);

int vegBPin = 1;
char vegBGain = 0;
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> vegBBase(SIN8192_DATA);

int vegCPin = 2;
char vegCGain = 0;
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> vegCBase(SIN8192_DATA);

int vegDPin = 3;
char vegDGain = 0;
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> vegDBase(SIN8192_DATA);

int vegEPin = 4;
char vegEGain = 0;
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> vegEBase(SIN8192_DATA);

int vegFPin = 5;
char vegFGain = 0;
Oscil<SIN8192_NUM_CELLS, AUDIO_RATE> vegFBase(SIN8192_DATA);

// The delay to use between sending vegetable resistance data to the computer.
EventDelay dataPollDelay(CONTROL_RATE);

void setup(){
  Serial.begin(9600);

  startMozzi(CONTROL_RATE);
  dataPollDelay.set(POLL_RATE);

  setupFastADC();
  pinMode(vegAPin, INPUT);
  pinMode(vegBPin, INPUT);
  pinMode(vegCPin, INPUT);
  pinMode(vegDPin, INPUT);
  pinMode(vegEPin, INPUT);
  pinMode(vegFPin, INPUT);

  vegABase.setFreq(360u);  
  vegBBase.setFreq(360u);
  vegCBase.setFreq(360u);
  vegDBase.setFreq(360u);
  vegEBase.setFreq(360u);
  vegFBase.setFreq(360u);
}

void loop() {
  audioHook();
}

/**
 * Calculates the resistance of the vegetable in k-Ohms (based on a 5 volt input).
 */
float calcOhm(int analogValue) {
  float vIn = 5.0f;
  float vOut = (analogValue / 1023.0f) * vIn;
  return (((10.0f * vIn) / vOut) - 10.0f);
}

/**
 * Works out what tone to use for the vegetable based on the input value.
 */
unsigned int calcFreq(int analogValue) {
  return ((((analogValue * 4) / 1023.0f) * 360u) + 220u);
}

void printVeg(String name, int pin) {
    Serial.print(name);
    Serial.print(analogRead(pin));
    Serial.print("\n");  
}

void updateControl() {
  if (dataPollDelay.ready()) {
    printVeg("VegA,", vegAPin);
    vegABase.setFreq(calcFreq(analogRead(vegAPin)));
    printVeg("VegB,", vegBPin);
    vegBBase.setFreq(calcFreq(analogRead(vegBPin)));
    printVeg("VegC,", vegCPin);
    vegCBase.setFreq(calcFreq(analogRead(vegCPin)));    
    printVeg("VegD,", vegDPin);
    vegDBase.setFreq(calcFreq(analogRead(vegDPin)));

    dataPollDelay.start();
  }  
}

int updateAudio() {
  int result = (vegABase.next() * (digitalRead(vegAPin + 2) != HIGH))
             + (vegBBase.next() * (digitalRead(vegBPin + 2) != HIGH))
             + (vegCBase.next() * (digitalRead(vegCPin + 2) != HIGH))
             + (vegDBase.next() * (digitalRead(vegDPin + 2) != HIGH))
  ;

  return result;
}
