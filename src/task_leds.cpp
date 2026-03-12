#include "task_leds.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>

void envoyerAuShifter(uint8_t data) {
  digitalWrite(PIN_SHIFT_LATCH, LOW);
  shiftOut(PIN_SHIFT_DATA, PIN_SHIFT_CLK, MSBFIRST, data);
  digitalWrite(PIN_SHIFT_LATCH, HIGH);
}

static uint8_t couleurWifiEnAttente(uint8_t step) {
  switch (step % 3) {
    case 0: return RGB1_RED;                       // rouge
    case 1: return RGB1_RED | RGB1_GREEN;          // orange
    default: return RGB1_GREEN;                    // vert
  }
}

static uint8_t couleurSignalRssi(long rssi) {
  // Gradient simple sur la 2e LED :
  // très faible   : rouge
  // moyen-faible : rouge + vert (orange)
  // bon          : vert
  // excellent    : vert + bleu (cyan)
  if (rssi >= -50) {
    return RGB2_GREEN | RGB2_BLUE;
  } else if (rssi >= -60) {
    return RGB2_GREEN;
  } else if (rssi >= -70) {
    return RGB2_RED | RGB2_GREEN;
  } else {
    return RGB2_RED;
  }
}

void taskLEDs(void* pvParameters) {
  uint8_t animStep = 0;

  for (;;) {
    wl_status_t status = WiFi.status();

    if (status != WL_CONNECTED) {
      // En attente de connexion WiFi :
      // LED 1 : animation rouge -> orange -> vert
      // LED 2 : éteinte (aucune info de signal tant que pas connecté)
      uint8_t out = couleurWifiEnAttente(animStep++);
      digitalWrite(LED_BUILTIN, (animStep & 0x01) ? HIGH : LOW);
      envoyerAuShifter(out);
      vTaskDelay(pdMS_TO_TICKS(300));
    } else {
      // WiFi connecté :
      // LED 1 : verte fixe pour indiquer la connexion
      // LED 2 : gradient de couleur en fonction du RSSI
      long rssi = WiFi.RSSI();
      uint8_t out = RGB1_GREEN | couleurSignalRssi(rssi);

      digitalWrite(LED_BUILTIN, HIGH);
      envoyerAuShifter(out);
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}
