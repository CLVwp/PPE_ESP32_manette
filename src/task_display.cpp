#include "task_display.h"
#include "config.h"
#include "joy_shared.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

bool display_begin() {
  Wire.begin(OLED_SDA, OLED_SCL);
  return display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
}

void taskDisplay(void* pvParameters) {
  static const char* const actionsMode1[] = {
    "Av.G", "Av", "Av.D", "Tr.G", "Idle", "Tr.D", "Re.G", "Rec", "Re.D"
  };
  static const char* const actionsMode2[] = {
    "IAv.G", "IAv", "IAv.D", "IG", "Idle", "ID", "IAr.G", "IAr", "IAr.D"
  };
  unsigned long lastDebounce = 0;

  for (;;) {
    if (buttonPressedFlag && (millis() - buttonPressTime) > DEBOUNCE_MS && (millis() - lastDebounce) > DEBOUNCE_MS) {
      buttonPressedFlag = false;
      lastDebounce = millis();
      joystickMode = (joystickMode == 1) ? 2 : 1;
    }

    int posX, posY, col, row, action;
    uint8_t mode;
    if (mutexJoy != NULL && xSemaphoreTake(mutexJoy, pdMS_TO_TICKS(20)) == pdTRUE) {
      posX = joyData.posX;
      posY = joyData.posY;
      col = joyData.col;
      row = joyData.row;
      action = joyData.action;
      xSemaphoreGive(mutexJoy);
    } else {
      posX = SCREEN_W / 2;
      posY = SCREEN_H / 2;
      col = 1;
      row = 1;
      action = 4;
    }
    mode = joystickMode;

    const int cw = SCREEN_W / 3;
    const int ch = (SCREEN_H - 10) / 3;
    const int gridH = ch * 3;
    const char* const* actions = (mode == 1) ? actionsMode1 : actionsMode2;

    display.invertDisplay(mode == 2);
    display.clearDisplay();

    for (int i = 1; i < 3; i++) {
      display.drawFastVLine(i * cw, 0, gridH, SSD1306_WHITE);
      display.drawFastHLine(0, i * ch, SCREEN_W, SSD1306_WHITE);
    }
    display.drawFastHLine(0, gridH, SCREEN_W, SSD1306_WHITE);

    display.fillRect(col * cw + 1, row * ch + 1, cw - 1, ch - 1, SSD1306_WHITE);
    display.setTextSize(1);
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        int idx = r * 3 + c;
        bool active = (c == col && r == row);
        display.setTextColor(active ? SSD1306_BLACK : SSD1306_WHITE);
        display.setCursor(c * cw + 2, r * ch + 2);
        display.print(actions[idx]);
        display.setTextColor(SSD1306_WHITE);
      }
    }
    display.setCursor(0, gridH + 1);
    display.print(mode == 1 ? F("M1 ") : F("M2 "));
    display.print(F("X:"));
    display.print(posX);
    display.print(F(" Y:"));
    display.print(posY);
    display.display();

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
