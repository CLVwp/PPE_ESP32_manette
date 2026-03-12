#include "joy_shared.h"
#include <Arduino.h>

volatile JoyShared joyData = {0};
volatile uint8_t joystickMode = 1;
volatile bool buttonPressedFlag = false;
volatile unsigned long buttonPressTime = 0;
SemaphoreHandle_t mutexJoy = NULL;

void IRAM_ATTR onJoyButtonISR() {
  buttonPressedFlag = true;
  buttonPressTime = millis();
}
