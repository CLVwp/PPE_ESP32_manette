#ifndef TASK_LEDS_H
#define TASK_LEDS_H

#include <Arduino.h>

/** Tâche FreeRTOS : clignotement LED + shift register 74HC595. */
void taskLEDs(void* pvParameters);

/** Envoie un octet au shift register (pour état initial dans setup). */
void envoyerAuShifter(uint8_t data);

#endif
