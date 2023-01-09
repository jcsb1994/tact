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
//
// Description:
//    This file provides a high level interface for tact switch input management.
//    Captures 3 types of events: button press, button release, and long press.
//
//    Events are returned from the poll function.
//    A callback function can be set to be called when an event occurs.
//
//***********************************************************************************

#ifndef TACT_H
#define TACT_H

#include <stdint.h>
#include <stdbool.h>

#define TACT_DFLT_DEBOUNCE_PERIOD_MS    (100)
#define TACT_DFLT_TIME_TO_LONG_PRESS_MS (1000)

typedef enum {
    TACT_NO_PRESS,
    TACT_SHORT_PRESS,
    TACT_RELEASE_PRESS,
    TACT_LONG_PRESS
} tact_press_t;
class tact
{
public:
    /*! @param assigned_pin Gpio to which the tact button is connected to
        @param read_gpio_cb Hardware specific gpio read function that takes gpio # as argument and returns its state 
        @param poll_freq_hz Frequency at which the tact obect will be polled using tact.poll()
        @param active_state Active state of the tact, (e.g. active high or active low),
            i.e. return value of the read_gpio_cb callback when the tact is pressed */
    tact(int assigned_pin,
         int (*read_gpio_cb)(int /* gpio */),
         uint16_t poll_freq_hz,
         int active_state);

    /*! @brief Poll the tact button to detect presses. Must call repeatedly in main loop */
    tact_press_t poll(void (*shortPressCb)() = 0, void (*releaseCb)() = 0, void (*longPressCb)() = 0);

    void setPin(int assigned_pin) { tact::_pin = assigned_pin; }
    int  getPin() { return tact::_pin; }
    /*! @brief Set the active state of the tact, (e.g. active high or active low),
        i.e. return value of the read_gpio callback when the tact is pressed */
    void setActiveState(int state) { _active_state = state; };

    /*! @brief Set the frequency at which tact.poll() is called to adjust the debounce algorithm */
    void setPollFreq(uint16_t hertz) { _poll_freq_hz = hertz; _setMaxDebounceCount(); }
    /*! @note The debounce period determines how long before a button press is detected.
    A wobbly button may need a longer debounce period, but a value above 100-150ms will be noticeable */
    void setDebouncePeriod(uint16_t millisec) { _debounce_time_ms = millisec; _setMaxDebounceCount(); }
    /*! @brief Set time period the user needs to press the button to trigger a long press effect */
    void setTimeToLongPress(uint16_t millisec) {_ticks_to_long_press = _msToTicks(millisec); }

private:
    // Press settings
    int _pin;
    int (*_read_gpio_cb)(int);
    uint16_t _poll_freq_hz;
    int _active_state;
    uint16_t _ticks_to_long_press;
    // Press algo
    uint16_t _long_press_ticks = 0;
    bool _long_effect_done = false;
    bool _is_pressed = false;

    // Debounce settings
    uint16_t _debounce_time_ms = TACT_DFLT_DEBOUNCE_PERIOD_MS;
    uint16_t _max_debounce_count;
    // Debounce algo
    uint16_t _debounce_counter = 0;
    bool _curr_debounced_state = false; // Output of the algorithm, true for active (pressed)
    bool _last_debounced_state = false;

    void _init();
    bool _read();
    void _debounce();
    void _setMaxDebounceCount();
    bool _isNowReleased();
    bool _isNowPressed();
    bool _isLongPressReached();
    uint16_t _msToTicks(uint16_t period_ms);
};

#endif