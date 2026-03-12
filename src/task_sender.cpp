#include "task_sender.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "config.h"
#include "joy_shared.h"

namespace {

WiFiUDP udp;

}  // namespace

void taskSender(void* pvParameters) {
  (void)pvParameters;

  JoyShared local{};
  uint8_t localMode = 0;

  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      if (mutexJoy != NULL && xSemaphoreTake(mutexJoy, pdMS_TO_TICKS(10)) == pdTRUE) {
        // Copier champ par champ depuis la structure volatile globale
        local.rawX   = joyData.rawX;
        local.rawY   = joyData.rawY;
        local.posX   = joyData.posX;
        local.posY   = joyData.posY;
        local.col    = joyData.col;
        local.row    = joyData.row;
        local.action = joyData.action;
        localMode    = joystickMode;
        xSemaphoreGive(mutexJoy);
      }

      // Format texte : "posX;posY;action;mode\n"
      char buffer[64];
      int len = snprintf(buffer, sizeof(buffer),
                         "%d;%d;%d;%u\n",
                         local.posX, local.posY, local.action, (unsigned)localMode);
      if (len > 0) {
        udp.beginPacket(RPI_IP, RPI_UDP_PORT);
        udp.write((const uint8_t*)buffer, (size_t)len);
        udp.endPacket();
      }
    }

    vTaskDelay(pdMS_TO_TICKS(JOY_SEND_INTERVAL_MS));
  }
}

