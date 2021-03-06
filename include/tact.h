//***********************************************************************************
// Copyright 2021 jcsb1994
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
//
// Description:
//    This file provides a high level interface for tact switch input management.
//    Captures 3 types of events: button press, button release, and long press.
//
//    Events are returned from the poll function.
//    A callback function can be set to be called when an event occurs. 
//    ***Events of value 0 are ignored.
//
//***********************************************************************************

#ifndef TACT_H
#define TACT_H

#include <Arduino.h>

#define DFLT_TACT_SHORT_PRESS_CODE   (1)
#define DFLT_TACT_RELEASE_PRESS_CODE (2)
#define DFLT_TACT_LONG_PRESS_CODE    (3)
#define DFLT_TACT_SAMPLE_FREQ_HZ (10000)  // 10k when not delaying
class tact
{
public:

//#######################################################################
// Constructors
//#######################################################################

    tact(int assigned_pin);

    tact(int assigned_pin, void (*eventCallback)(int), int short_press_event = 0,
         int release_press_event = 0,
         int long_press_event = 0);

//#######################################################################
// Public functions
//#######################################################################

    int poll();

    void setPin(int assigned_pin) { tact::_pin = assigned_pin; }
    int  getPin() { return tact::_pin; }

    void setSamplingFreqkHz(uint16_t freq) { _sampling_freq_hz = freq; _setMaxDebounce(); }
    void setDebouncePeriodMs(uint16_t period) { _debounce_time_ms = period; _setMaxDebounce(); }

    void setActiveState(bool state);

private:
    int _pin;

    void (*_eventCallback)(int);

    int _shortPressEvent   = DFLT_TACT_SHORT_PRESS_CODE;
    int _releasePressEvent = DFLT_TACT_RELEASE_PRESS_CODE;
    int _longPressEvent    = DFLT_TACT_LONG_PRESS_CODE;

    bool _active_state = 0;

    // Debounce variables
    uint32_t _sampling_freq_hz = DFLT_TACT_SAMPLE_FREQ_HZ; // will need logic to check when readings are too fast
    uint16_t _debounce_time_ms = 300;
    uint16_t _maxDebounce;

    uint16_t _long_press_delay_ms = 1000;

    volatile uint16_t _inputIntegrator;
    volatile bool _curr_debounced_input; // Output of the algorithm
    volatile bool _last_debounced_input;

    unsigned long long_press_counter = 0;
    bool long_effect_done = 0;
    bool is_pressed = false; 

//#######################################################################
// Private functions
//#######################################################################

    void _init();

    uint8_t _read(); // can be reimplemented for different hardware

    void _debounce();

    void _setMaxDebounce();
    bool _isNowReleased();
    bool _isNowPressed();
};

#endif // Header guard