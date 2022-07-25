#include "tact.h"

#define DEBUG_TACT 0
#if DEBUG_TACT
#define PRINT(msg) Serial.println(msg)
#else
#define PRINT(msg) 
#endif

//#######################################################################
// Private functions
//#######################################################################

void tact::_init()
{
    _setMaxDebounce();
    
    _curr_debounced_input = !_active_state;
    _last_debounced_input = !_active_state;

    pinMode(_pin, INPUT_PULLUP);
}

uint8_t tact::_read()
{
    return digitalRead(_pin);
}

void tact::_setMaxDebounce() {
        _maxDebounce = ((tact::_sampling_freq_hz * tact::_debounce_time_ms) >> 10); // divides in 1k for ms to s
    if (!_maxDebounce)
    {
        _maxDebounce = 1;
    }
}

void tact::_debounce()
{
    uint8_t rawInput = _read();

    if (rawInput == 0)
    {
        if (_inputIntegrator > 0) {
            _inputIntegrator--;
        }  
    }
    else if (_inputIntegrator < _maxDebounce) {
        _inputIntegrator++;
    }
        

    if (_inputIntegrator == 0)
        _curr_debounced_input = 0;
    else if (_inputIntegrator >= _maxDebounce)
    {
        _inputIntegrator = _maxDebounce; // Defensive code
        _curr_debounced_input = 1;
    }
}

bool tact::_isNowReleased()
{
    if ((_curr_debounced_input != _active_state) &&
        (_last_debounced_input == _active_state))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool tact::_isNowPressed()
{
    if ((_curr_debounced_input == _active_state) &&
        (_last_debounced_input != _active_state))
    {
        return true;
    }
    else
    {
        return false;
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

tact::tact(int assigned_pin) : _pin(assigned_pin)
{
    _buttonEventCallback = NULL;
    _init();
}

tact::tact(int assigned_pin, int short_press_event,
           int release_press_event/*  = 0 */,
           int long_press_event/*  = 0 */) : _pin(assigned_pin),
                                       _shortPressEvent(short_press_event),
                                       _releasePressEvent(release_press_event),
                                       _longPressEvent(long_press_event)
{
    _buttonEventCallback = NULL;
    _init();
}


tact::tact(int assigned_pin, void (*eventCallback)(int), int short_press_event/*  = 0 */,
           int release_press_event/*  = 0 */,
           int long_press_event/*  = 0 */) : _pin(assigned_pin),
                                       _buttonEventCallback(eventCallback),
                                       _shortPressEvent(short_press_event),
                                       _releasePressEvent(release_press_event),
                                       _longPressEvent(long_press_event)
{
    _init();
}


//#######################################################################
// Public functions
//#######################################################################

void tact::setActiveState(bool state)
{
    if (_active_state != state)
    {
        _curr_debounced_input = !_curr_debounced_input;
        _last_debounced_input = !_last_debounced_input;
    }
    _active_state = state;
}

int tact::poll(void (*shortPressCb)(), void (*releaseCb)(), void (*longPressCb)())
{
    int rc = 0;

    _debounce();

    //----------------------------------------------------------------

    if (_isNowPressed())
    {
        PRINT("pressed");
        is_pressed = true;
        long_press_counter = millis();

        if (shortPressCb) { shortPressCb(); }
        if (_shortPressEvent)
        {
            if (_buttonEventCallback != NULL){
                _buttonEventCallback(_shortPressEvent);
            } 
            rc = _shortPressEvent;
        }
    }

    //----------------------------------------------------------------

    else if (_isNowReleased())
    {
        PRINT("release (no trigger when long press)");
        if (!long_effect_done && _releasePressEvent)
        {
            if (releaseCb) { releaseCb(); }
            if (_buttonEventCallback != NULL)
                _buttonEventCallback(_releasePressEvent);
            rc = _releasePressEvent;
        }

        long_effect_done = 0;
        long_press_counter = 0;
        is_pressed = 0;
    }

//----------------------------------------------------------------

    else if (is_pressed && !long_effect_done && ((millis() - long_press_counter) >= _long_press_delay_ms))
    {
        PRINT("long pressed");
        if (longPressCb) { longPressCb(); }
        if (_longPressEvent) {
            if (_buttonEventCallback != NULL)
                _buttonEventCallback(_longPressEvent);
            rc = _longPressEvent;
        }

        long_effect_done = true;
        long_press_counter = 0;
    }

//----------------------------------------------------------------

    tact::_last_debounced_input = tact::_curr_debounced_input;

    return rc;
}
