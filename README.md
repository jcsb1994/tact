# tact
This library provides an interface to facilitate operations with tactile buttons in embedded devices. It has no dependency to any platform, however you must be able to compile C++11 on your platform.

This library supports detection of button presses, button releases, and long presses.

## Usage
### tact object
In order to interact with a button, the user must instantiate a `tact` object.

The object must be given:
1. A GPIO pin number, i.e. the gpio to which the button is connected to
2. A callback to the platform's gpio read function, or a wrapper with the form `int read(int gpio)`
3. The active state of the button (e.g. An active low button is pulled high when unpressed)
4. The frequency at which `tact.poll()` will be called.

### `tact.poll()` to detect presses

The library supports 2 ways of reporting presses.
1. The `tact.poll()` button returns a `tact_press_t` value other than `TACT_NO_PRESS` if any press occurs.
2. It is possible to pass callbacks or lambdas to the `tact.poll()` function, enabling the user to trigger a desired effect upon any press event.

The function `tact.poll()` must be called at a predetermined frequency. This helps the debouncing and long press detection algorithms behave as expected. This is why the constructor takes a `poll_freq` argument. Afterwards, you can use your system's timer to call `tact.poll()`

#### Example with Arduino

This library has no dependency to Arduino.

The example (available at `examples/tact_arduino_uno.cpp`) shows how you can use this library with a timed call to `tact.poll()` using `millis()`. It is also be possible to use an ISR.

