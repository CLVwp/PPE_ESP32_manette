#ifndef TASK_JOYSTICK_H
#define TASK_JOYSTICK_H

/** Tâche FreeRTOS : lecture ADC joystick, mise à jour de joyData (mutex). */
void taskJoystick(void* pvParameters);

#endif
