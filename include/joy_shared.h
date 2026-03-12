#ifndef JOY_SHARED_H
#define JOY_SHARED_H

#include <Arduino.h>
#include <freertos/semphr.h>

struct JoyShared {
  int rawX;
  int rawY;
  int posX;
  int posY;
  int col;
  int row;
  int action;
};

extern volatile JoyShared joyData;
extern volatile uint8_t joystickMode;
extern volatile bool buttonPressedFlag;
extern volatile unsigned long buttonPressTime;
extern SemaphoreHandle_t mutexJoy;

/** ISR à attacher au bouton joystick (FALLING). */
void IRAM_ATTR onJoyButtonISR();

#endif
