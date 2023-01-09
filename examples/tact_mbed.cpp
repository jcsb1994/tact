//***********************************************************************************
// Copyright 2023 jcsb1994
// Written by jcsb1994
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//***********************************************************************************
// Description:
//    Example for the tact library using Mbed OS
//    This example shows how you can implement a read_cb when the platform's
//    API (i.e. DigitalIn.read() ) does not suit the `int cb(int)` format
//***********************************************************************************

#include <mbed.h>
#include <tact.h>

#define TACT_POLL_FREQ_HZ 100
#define TACT_MS_BETWEEN_POLLS 10 // 1000ms/100Hz
#define NB_BUTTONS 2
#define BUTTON_ACTIVE_STATE 1

DigitalOut myLed(LED1);
Timer myTimer;
DigitalIn buttons[2] = {DigitalIn(BUTTON1),
                        DigitalIn(PD_0) };

int buttonRead(int pin)
{
  if (pin >= NB_BUTTONS) return !BUTTON_ACTIVE_STATE;
  int rc = buttons[pin].read();
  return rc;
}

// We declare tact objects pins as array offsets, not pins per sey
tact myTact = tact(0, buttonRead, TACT_POLL_FREQ_HZ, BUTTON_ACTIVE_STATE);

int main()
{ 
  myTimer.start();
  while(1) {
    if ((myTimer.read()*1000) >= TACT_MS_BETWEEN_POLLS) {
      myTact.poll([]{ myLed.write(1); },
                  []{ myLed.write(0); },
                  []{ myLed.write(1); });
      myTimer.reset();
    }
  }
}