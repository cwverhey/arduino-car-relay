#include "LowPower.h"

const int inputPin = 2;  // pin that supports external interrupt
const int outputPin = 10;  // any digital pin

const unsigned long delayTimeOn = 15000;  // in ms
const unsigned long delayTimeOff = 1000;  // in ms

bool lastInputState;
bool outputState = LOW;
unsigned long inputStateChangeTime;

// sleep routines
void wakeUpISR() {
  // empty ISR just to wake MCU
}

void startPowerDown() {
  // sleep until inputPin changes
  Serial.println("Going to sleep");
  Serial.flush();
  attachInterrupt(digitalPinToInterrupt(inputPin), wakeUpISR, CHANGE);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void endPowerDown() {
  detachInterrupt(digitalPinToInterrupt(inputPin));
  Serial.println("Woke up");
}

void powerDown() {
  startPowerDown();
  endPowerDown();
}

// save input state and set onboard LED
void updateInputState(bool newInputState) {
  inputStateChangeTime = millis();
  lastInputState = newInputState;
  digitalWrite(LED_BUILTIN, newInputState);
  Serial.print("Input changed to ");
  Serial.println(newInputState ? "HIGH" : "LOW");
}

// set new output state
void updateOutputState(bool newOutputState) {
    outputState = newOutputState;
    digitalWrite(outputPin, newOutputState);
    Serial.print("Output set to ");
    Serial.println(newOutputState ? "HIGH" : "LOW");
}

void setup() {
  pinMode(inputPin, INPUT);  // or INPUT_PULLUP without external pull-down
  pinMode(outputPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(outputPin, outputState);

  Serial.begin(9600);

  // blink onboard LED 3x
  for (int i = 1; i < 7; i++) {
    digitalWrite(LED_BUILTIN, i % 2);
    delay(200);
  }

  bool initialInputState = digitalRead(inputPin);
  updateInputState(initialInputState);

}

void loop() {

  bool inputState = digitalRead(inputPin);
  if (inputState != lastInputState) {
    updateInputState(inputState);
  }

  unsigned long elapsed = millis() - inputStateChangeTime;
  if (inputState == HIGH && outputState == LOW && elapsed >= delayTimeOn) {
    updateOutputState(HIGH);
  } else if (inputState == LOW && outputState == HIGH && elapsed >= delayTimeOff) {
    updateOutputState(LOW);
  }
  
  if (inputState == outputState) {
    // sleep until an interrupt wakes us up
    powerDown();
  } else {
    // keep polling until we need to update the output state
    delay(100);
  }

}
