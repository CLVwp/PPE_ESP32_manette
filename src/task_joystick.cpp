#include "task_joystick.h"
#include "config.h"
#include "joy_shared.h"
#include <Arduino.h>

void taskJoystick(void* pvParameters) {
  const int cx = SCREEN_W / 2, cy = SCREEN_H / 2;
  const int moveRadiusX = 64, moveRadiusY = 32;

  for (;;) {
    int rawX = analogRead(JOY_VRX);
    int rawY = analogRead(JOY_VRY);

    int dx, dy;
    if (rawX <= JOY_CENTER_X)
      dx = map(rawX, 0, JOY_CENTER_X, -moveRadiusY, 0);
    else
      dx = map(rawX, JOY_CENTER_X, 4095, 0, moveRadiusY);
    if (rawY <= JOY_CENTER_Y)
      dy = map(rawY, 0, JOY_CENTER_Y, moveRadiusX, 0);
    else
      dy = map(rawY, JOY_CENTER_Y, 4095, 0, -moveRadiusX);
    dx = constrain(dx, -moveRadiusY, moveRadiusY);
    dy = constrain(dy, -moveRadiusX, moveRadiusX);

    float nx = (float)dx / (float)moveRadiusY;
    float ny = (float)dy / (float)moveRadiusX;
    float dist = sqrtf(nx * nx + ny * ny);
    if (dist > 1.0f && dist > 0.001f) {
      float scale = 1.0f / dist;
      nx *= scale;
      ny *= scale;
      dx = (int)(nx * (float)moveRadiusY);
      dy = (int)(ny * (float)moveRadiusX);
    }

    int dxRot = -dy, dyRot = dx;
    int posX = cx - dxRot, posY = cy + dyRot;
    posX = cx + (int)((float)(posX - cx) * JOY_MARGIN_FACTOR);
    posY = cy + (int)((float)(posY - cy) * JOY_MARGIN_FACTOR);
    posX = constrain(posX, 0, SCREEN_W - 1);
    posY = constrain(posY, 0, SCREEN_H - 1);

    int col = constrain((posX * 3) / SCREEN_W, 0, 2);
    int row = constrain((posY * 3) / SCREEN_H, 0, 2);
    int action = row * 3 + col;

    if (mutexJoy != NULL && xSemaphoreTake(mutexJoy, pdMS_TO_TICKS(10)) == pdTRUE) {
      joyData.rawX = rawX;
      joyData.rawY = rawY;
      joyData.posX = posX;
      joyData.posY = posY;
      joyData.col = col;
      joyData.row = row;
      joyData.action = action;
      xSemaphoreGive(mutexJoy);
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
