#include "tact.h"

//#######################################################################
// Private functions
//#######################################################################

void tact::_init()
{
    _maxDebounce = ((tact::_sampling_freq_hz * tact::_debounce_time_ms) >> 10);
    if (!_maxDebounce) {
        _maxDebounce = 1;
    }
    pinMode(_pin, INPUT_PULLUP);
}

uint8_t tact::_read()
{
    return digitalRead(_pin);
}

void tact::_debounce()
{
    _rawInput = _read();

    if (_rawInput == 0)
    {
        if (_inputIntegrator > 0)
            _inputIntegrator--;
    }
    else if (_inputIntegrator < _maxDebounce)
        _inputIntegrator++;

    if (_inputIntegrator == 0)
        _curr_debounced_input = 0;
    else if (_inputIntegrator >= _maxDebounce)
    {
        _inputIntegrator = _maxDebounce; // Defensive code
        _curr_debounced_input = 1;
    }
}

//#######################################################################
// Constructors
//#######################################################################

/**********************************************************************/
/*!
    @brief  Ctor for tacts
    @param  assigned_pin hardware GPIO linked to the tact switch
*/
/**********************************************************************/

tact::tact(int assigned_pin) : _eventCallback(NULL), _pin(assigned_pin)
{
    _init();
}

tact::tact(int assigned_pin, void (*eventCallback)(int), int short_press_event = 0,
         int release_press_event = 0,
         int long_press_event = 0) : _pin(assigned_pin),
                                     _eventCallback(eventCallback),
                                     _shortPressEvent(short_press_event),
                                     _releasePressEvent(release_press_event),
                                     _longPressEvent(long_press_event)
{
    _init();
}

//#######################################################################
// Public functions
//#######################################################################

int tact::poll()
{
    _debounce();

    tact::_state = TACT_S_IDLE; // no need to reset every poll, should be done when storing in eventStorage

    //----------------------------------------------------------------

    if (!_curr_debounced_input && _last_debounced_input && !is_pressed)
    {
        is_pressed = true;
        long_press_counter = millis();
        tact::_state = TACT_S_SHORT_PRESS;
        _eventCallback(TACT_S_SHORT_PRESS);
    }

    //----------------------------------------------------------------

    if (!_last_debounced_input && _curr_debounced_input && is_pressed)
    {
        if (!long_effect_done) {
            tact::_state = TACT_S_RELEASE_PRESS;
            _eventCallback(TACT_S_SHORT_PRESS);
        }

        long_effect_done = 0;
        long_press_counter = 0;
        is_pressed = 0;
    }

    //----------------------------------------------------------------

    else if (is_pressed && !long_effect_done && ((millis() - long_press_counter) >= _long_press_delay_ms))
    {
        tact::_state = TACT_S_LONG_PRESS;
        _eventCallback(TACT_S_LONG_PRESS);
        long_effect_done++;
        long_press_counter = 0;
    }

    //----------------------------------------------------------------

    tact::_last_debounced_input = tact::_curr_debounced_input;

    return tact::_state;
}
