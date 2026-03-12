#include "system_info.h"
#include <Arduino.h>
#include <esp_system.h>

static const char* resetReasonStr(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_UNKNOWN:   return "Inconnu";
    case ESP_RST_POWERON:   return "Alimentation";
    case ESP_RST_EXT:       return "Pin externe";
    case ESP_RST_SW:        return "Logiciel";
    case ESP_RST_PANIC:     return "Panic/Exception";
    case ESP_RST_WDT:       return "Watchdog";
    case ESP_RST_DEEPSLEEP: return "Réveil deep sleep";
    case ESP_RST_BROWNOUT:  return "Brownout";
    case ESP_RST_SDIO:      return "SDIO";
    default:                return "?";
  }
}

void printSystemInfo() {
  Serial.println("\n=== Test ESP32 ===");
  Serial.println("Chip: " + String(ESP.getChipModel()) + " (rev " + String(ESP.getChipRevision()) + ")");
  Serial.println("Cores: " + String(ESP.getChipCores()));
  Serial.println("Freq CPU: " + String(ESP.getCpuFreqMHz()) + " MHz");
  Serial.println("Flash: " + String(ESP.getFlashChipSize() / 1024) + " KB @ " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz");
  Serial.println("Heap total: " + String(ESP.getHeapSize() / 1024) + " KB");
  Serial.println("Heap libre: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("Heap min libre (depuis boot): " + String(ESP.getMinFreeHeap()) + " bytes");
  if (ESP.getPsramSize() > 0) {
    Serial.println("PSRAM total: " + String(ESP.getPsramSize() / 1024) + " KB");
    Serial.println("PSRAM libre: " + String(ESP.getFreePsram()) + " bytes");
  } else {
    Serial.println("PSRAM: non disponible");
  }
  uint64_t mac = ESP.getEfuseMac();
  Serial.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
    (uint8_t)(mac >> 40) & 0xFF, (uint8_t)(mac >> 32) & 0xFF, (uint8_t)(mac >> 24) & 0xFF,
    (uint8_t)(mac >> 16) & 0xFF, (uint8_t)(mac >> 8) & 0xFF, (uint8_t)mac & 0xFF);
  Serial.println("SDK: " + String(ESP.getSdkVersion()));
  Serial.println("Raison redémarrage: " + String(resetReasonStr(esp_reset_reason())));
  Serial.println("==================\n");
}
