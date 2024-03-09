// (c) Copyright 2022 Aaron Kimball, Ali Shah
// This file is open source under the terms of the BSD 3-Clause license.
//
// Example basic usage for the debounce library.

#include <Arduino.h>
#include <debounce.h>

// Connect a button to some GPIO pin; digital pin 6 in this example.
static constexpr int PIN = 6;


static void buttonHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    Serial.println("Button pressed");
  } else if (btnState == BTN_TAP_SINGLE) {
    // btnState == BTN_OPEN.
    Serial.println("Released button: single");
  } else if (btnState == BTN_TAP_DOUBLE) {
    // btnState == BTN_OPEN.
    Serial.println("Released button: double");
  } else if (btnState == BTN_TAP_TRIPPLE) {
    // btnState == BTN_OPEN.
    Serial.println("Released button: tripple");
  } else if (btnState == BTN_LONG_PRESS) {
    // btnState == BTN_OPEN.
    Serial.println("Button long pressed");
  }
}

// Define your button with a unique id (0) and handler function.
// (The ids are so one handler function can tell different buttons apart if necessary.)
static Button myButton(0, buttonHandler);

void setup() {
  Serial.begin(115200);
  pinMode(PIN, INPUT_PULLUP);

}

static void pollButtons() {
  myButton.update(digitalRead(PIN));
}

void loop() {
  pollButtons();
  delay(10);
}
