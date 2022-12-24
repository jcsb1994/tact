#include <Arduino.h>
#include "tact.h"

#define TACT_GPIO (7)
#define TACT_POLL_FREQ_HZ (50u)

tact myTact = tact(TACT_GPIO, digitalRead, TACT_POLL_FREQ_HZ, 0);

void setup() {
  Serial.begin(9600);
  Serial.println("hello");
}

void loop()
{
  static unsigned long last_poll = 0;
  static unsigned int millisec_between_polls = 1000/TACT_POLL_FREQ_HZ;

  if (millis() - last_poll >= millisec_between_polls)
  {
      myTact.poll([]
                  { Serial.println("Pressed!"); },
                  []
                  { Serial.println("Released!"); },
                  []
                  { Serial.println("Long press!"); });

      last_poll += millisec_between_polls;
  }
}
