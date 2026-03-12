#include <Arduino.h>
#include "system_info.h"
#include "config.h"
#include "joy_shared.h"
#include "task_joystick.h"
#include "task_display.h"
#include "task_leds.h"
#include "wifi_manager.h"

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println(F("\nBoot ESP32"));

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_SHIFT_DATA, OUTPUT);
  pinMode(PIN_SHIFT_CLK, OUTPUT);
  pinMode(PIN_SHIFT_LATCH, OUTPUT);
  pinMode(JOY_SW, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, LOW);
  envoyerAuShifter(RGB1_RED | RGB1_BLUE | RGB2_RED | RGB2_BLUE);

  mutexJoy = xSemaphoreCreateMutex();
  if (mutexJoy == NULL) {
    Serial.println(F("ERR mutex"));
    return;
  }

  attachInterrupt(digitalPinToInterrupt(JOY_SW), onJoyButtonISR, FALLING);

  if (!display_begin()) {
    Serial.println(F("OLED 0x3C non trouvé"));
  } else {
    Serial.println(F("OLED 0x3C OK"));
  }
  printSystemInfo();

  xTaskCreatePinnedToCore(taskJoystick, "Joy", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(taskDisplay, "Disp", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskLEDs, "LED", 2048, NULL, 0, NULL, 0);
  startWifiTask();

  Serial.println(F("RTOS: 3 tâches + WiFi + ISR bouton"));
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
