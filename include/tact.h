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
//
//***********************************************************************************

#ifndef TACT_H
#define TACT_H

#include <Arduino.h>

//#######################################################################
// Macros
//#######################################################################

#define TACT_PIN_UNASSIGNED (-1)

//#######################################################################
// Tact
//#######################################################################

class tact
{
public:
    typedef enum
    {
        TACT_S_IDLE,
        TACT_S_SHORT_PRESS,
        TACT_S_RELEASE_PRESS,
        TACT_S_LONG_PRESS
    } TactState_t;

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

    void setSamplingFreqkHz(uint16_t freq) { _sampling_freq_hz = freq; }
    void setDebouncePeriodMs(uint16_t period) { _debounce_time_ms = period; }

    void setActiveState(bool state);

private:
    int _pin;

    uint16_t _long_press_delay_ms = 1000;
    bool _active_state = 0;

    TactState_t _state = TACT_S_IDLE; // carries the current state after polling

    void (*_eventCallback)(int);

    int _shortPressEvent;
    int _releasePressEvent;
    int _longPressEvent;

    // Debounce variables
    uint32_t _sampling_freq_hz = 10000; // will need logic to check when readings are too fast
    uint16_t _debounce_time_ms = 300;
    uint16_t _maxDebounce;

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

    bool _isNowReleased();
    bool _isNowPressed();
};

#endif // Header guard