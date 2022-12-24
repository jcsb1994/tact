#include "tact.h"

#ifndef TACT_DEBUG_PRINT // user can define to print debug messages
#define TACT_DEBUG_PRINT(msg) 
#endif

//#######################################################################
// Private functions
//#######################################################################

bool tact::_read()
{
    return (_active_state == _read_gpio_cb(_pin));
}

void tact::_setMaxDebounceCount()
{
    _max_debounce_count = ((tact::_poll_freq_hz * tact::_debounce_time_ms) / 1000);
    if (!_max_debounce_count)
    {
        _max_debounce_count = 1;
    }
}

void tact::_debounce()
{
    bool rawInput = _read();

    if (rawInput == false)
    {
        if (_debounce_counter > 0) {
            _debounce_counter--;
            TACT_DEBUG_PRINT("--");
            TACT_DEBUG_PRINT(_debounce_counter);
        }  
    }
    else if (_debounce_counter < _max_debounce_count) {
        _debounce_counter++;
        TACT_DEBUG_PRINT("++");
        TACT_DEBUG_PRINT(_debounce_counter);
    }
        
    if (_debounce_counter == 0)
    {
        _curr_debounced_state = false;
    }
    else if (_debounce_counter >= _max_debounce_count)
    {
        _debounce_counter = _max_debounce_count; // Defensive code
        _curr_debounced_state = true;
    }
}

bool tact::_isNowReleased()
{
    return (!_curr_debounced_state) && (_last_debounced_state);
}

bool tact::_isNowPressed()
{
    return (_curr_debounced_state) && (!_last_debounced_state);
}

bool tact::_isLongPressReached()
{
    return (_is_pressed && (_long_press_ticks >= _ticks_to_long_press));
}

uint16_t tact::_msToTicks(uint16_t period_ms)
{
    return (period_ms * _poll_freq_hz) / 1000;
}

//#######################################################################
// Public functions
//#######################################################################

tact::tact(int assigned_pin, int (*read_gpio_cb)(int), uint16_t poll_freq_hz, int active_state) : 
    _pin(assigned_pin), _read_gpio_cb(read_gpio_cb),
    _poll_freq_hz(poll_freq_hz), _active_state(active_state)
{
    setTimeToLongPress(TACT_DFLT_TIME_TO_LONG_PRESS_MS);
    _setMaxDebounceCount();
}


tact_press_t tact::poll(void (*shortPressCb)(), void (*releaseCb)(), void (*longPressCb)())
{
    tact_press_t rc = TACT_NO_PRESS;    
    _debounce();
    if (_is_pressed) { _long_press_ticks++; }

    if (_isNowPressed())
    {
        TACT_DEBUG_PRINT("pressed pin:");
        TACT_DEBUG_PRINT(_pin);
        _is_pressed = true;
        _long_press_ticks = 0;

        if (shortPressCb != 0) { shortPressCb(); }
        rc = TACT_SHORT_PRESS;
    }

    else if (_isNowReleased())
    {
        TACT_DEBUG_PRINT("released pin:");
        TACT_DEBUG_PRINT(_pin);
        if (!_long_effect_done) // no effect if long press occured
        {
            if (releaseCb != 0) { releaseCb(); }
            rc = TACT_RELEASE_PRESS;
        }
        _long_press_ticks = 0;
        _long_effect_done = false;
        _is_pressed = false;
    }
    else if (_isLongPressReached() && !_long_effect_done)
    {
        TACT_DEBUG_PRINT("long press on pin:");
        TACT_DEBUG_PRINT(_pin);
        if (longPressCb != 0) { longPressCb(); }
        rc = TACT_LONG_PRESS;
        
        _long_press_ticks = 0;
        _long_effect_done = true;
    }

    _last_debounced_state = _curr_debounced_state;
    return rc;
}
