#include "wifi_manager.h"

#include <Arduino.h>
#include <WiFi.h>

#include "config.h"

namespace {

bool wifiTaskStarted = false;

const char* wifiStatusToString(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS: return "IDLE";
    case WL_NO_SSID_AVAIL: return "SSID introuvable";
    case WL_SCAN_COMPLETED: return "scan termine";
    case WL_CONNECTED: return "connecte";
    case WL_CONNECT_FAILED: return "echec authentification";
    case WL_CONNECTION_LOST: return "connexion perdue";
    case WL_DISCONNECTED: return "deconnecte";
    default: return "etat inconnu";
  }
}

void taskWifi(void* pvParameters) {
  wl_status_t lastStatus = WL_IDLE_STATUS;
  bool attemptInProgress = false;
  bool ipPrinted = false;
  unsigned long connectStartedAt = 0;
  unsigned long lastRetryAt = 0;

  vTaskDelay(pdMS_TO_TICKS(WIFI_START_DELAY_MS));

  WiFi.persistent(false);
  WiFi.setSleep(false);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  Serial.printf("WiFi: demarrage differe, connexion a \"%s\"\n", WIFI_SSID);

  for (;;) {
    const wl_status_t status = WiFi.status();

    if (status != lastStatus) {
      Serial.printf("WiFi: statut %s\n", wifiStatusToString(status));
      lastStatus = status;
    }

    if (status == WL_CONNECTED) {
      attemptInProgress = false;
      if (!ipPrinted) {
        ipPrinted = true;
        Serial.print("WiFi: connecte, IP = ");
        Serial.println(WiFi.localIP());
      }
    } else {
      const unsigned long now = millis();
      ipPrinted = false;

      if (!attemptInProgress) {
        if (lastRetryAt == 0 || (now - lastRetryAt) >= WIFI_RETRY_DELAY_MS) {
          Serial.printf("WiFi: tentative de connexion a \"%s\"\n", WIFI_SSID);
          WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
          attemptInProgress = true;
          connectStartedAt = now;
        }
      } else if ((now - connectStartedAt) >= WIFI_CONNECT_TIMEOUT_MS) {
        Serial.printf("WiFi: timeout apres %lu ms, nouvel essai plus tard\n", WIFI_CONNECT_TIMEOUT_MS);
        WiFi.disconnect();
        attemptInProgress = false;
        lastRetryAt = now;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

}  // namespace

void startWifiTask() {
  if (wifiTaskStarted) {
    return;
  }

  wifiTaskStarted = true;
  xTaskCreatePinnedToCore(taskWifi, "WiFi", 4096, NULL, 1, NULL, 0);
}

