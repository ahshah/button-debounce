// (c) Copyright 2022 Aaron Kimball

#include "debounce.h"

/** A button handler that does nothing; for states when a given button is unmapped. */
void emptyBtnHandler(uint8_t btnId, uint8_t btnState) {
}

Button::Button() : Button(0, NULL) {
}

Button::Button(uint8_t id, buttonHandler_t handlerFn):
    _id(id), _curState(BTN_TAP_SINGLE), _priorPoll(BTN_TAP_SINGLE), _readStartTime(0),
    _pressCount(0),
    _tapCount(0),
    _tapStartTime(0),
    _pressStartTime(0),
    _longPressWindow(1000),
    _tapExpiryWindow(100),
    _pushDebounceInterval(BTN_DEBOUNCE_MILLIS),
    _releaseDebounceInterval(BTN_DEBOUNCE_MILLIS),
    _handlerFn(handlerFn) {

  if (NULL == _handlerFn) {
    _handlerFn = emptyBtnHandler;
  }
}

bool Button::update(uint8_t latestPoll) {
  latestPoll = (latestPoll != 0); // Collapse input into a 1/0 universe.
  if (latestPoll != _priorPoll) {
    // Input has changed since we last polled. Reset debounce timer.
    _readStartTime = millis();
  }

  // Save reading for next interrogation of update().
  _priorPoll = latestPoll;

  // Decide which debounce interval to use, depending on whether we're monitoring
  // for a next state change of "push" (0 to 1) or "release" (1 to 0).
  unsigned int debounceInterval = _pushDebounceInterval;
  if (_curState == BTN_PRESSED) {
    debounceInterval = _releaseDebounceInterval;
  }

  if ((millis() - _readStartTime) > debounceInterval) {
    // The reading has remained consistent for the debounce interval.
    // It's a legitimate state.

    if (latestPoll != _curState) {
      _curState = latestPoll; // Lock this in as the new state.
      return true;
    }
  }

  return false; // No state change.
}


bool Button::updateDoubleTap(uint8_t latestPoll) {
    bool rc = Button::update(latestPoll);
    unsigned long ms = millis();

    // if we noticed no events  and have no outstanding tap and press events to resolve 
    if (!rc && _tapCount == 0 && _pressCount == 0) {
        return rc;
    }

    // if we noticed no events, but have outstanding taps events resolve
    else if (!rc && _tapCount > 0 ) {
        // we are inside the tap window but have noticed no events, nothing to but wait
        if ((ms- _tapStartTime) < _tapExpiryWindow) {
            return rc;
        }
        // we are now outside of the tap window, select the appropriate call back and reset the tap state
        switch(_tapCount) {
            case 1:
                (*_handlerFn)(_id, BTN_TAP_SINGLE);
                break;
            case 2:
                (*_handlerFn)(_id, BTN_TAP_DOUBLE);
                break;
            case 3:
                (*_handlerFn)(_id, BTN_TAP_TRIPPLE);
                break;
            default:
                break;
        }
        _tapCount  = 0;
        _tapStartTime = 0;
        return rc;
    }
    else if (!rc && _pressCount > 0 ) {
        // we are inside the long press window but have noticed no events, nothing to do but wait
        if ((ms- _pressStartTime) < _longPressWindow) {
            return rc;
        }

        // we are now outside of the long press window, call the call back and
        // prevent futher call backs from firing until the next key down event
        _pressCount = 0;
        _pressStartTime = 0;
        (*_handlerFn)(_id, BTN_LONG_PRESS); 
        return rc;
    }

    // we have noticed an event, and its a key down event
    else if (rc && _curState == BTN_PRESSED) {
        //(*_handlerFn)(_id, _curState); 
        // reset the tap and press windows, allowing us to keep the expiry window short
        _tapStartTime = ms;
        _pressStartTime = ms;
        _pressCount++;
        return rc;
    }

    // we have noticed a key up event
    if (_pressCount && (ms - _pressStartTime) < _longPressWindow) {
        _pressCount = 0;
        _pressStartTime = 0;
        (*_handlerFn)(_id, BTN_PRESSED); 
    } else {
        // already handled
    }

    // reset the tap window, this allows us to keep the expiry window short
    _tapStartTime = ms;
    _tapCount++; 
    return rc;
}
