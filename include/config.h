#ifndef CONFIG_H
#define CONFIG_H

#define LED_BUILTIN 2

// OLED I2C
#define OLED_SDA  21
#define OLED_SCL  22
#define OLED_ADDR 0x3C
#define SCREEN_W  128
#define SCREEN_H  64

// Joystick
#define JOY_VRX  34
#define JOY_VRY  35
#define JOY_SW   15
#define JOY_CENTER_X  2200
#define JOY_CENTER_Y  2200
#define JOY_MARGIN_FACTOR  0.95f
#define DEBOUNCE_MS  200

// Shift register 74HC595
#define PIN_SHIFT_DATA  13
#define PIN_SHIFT_CLK   12
#define PIN_SHIFT_LATCH 27
#define RGB1_RED   0b00001000
#define RGB1_GREEN 0b00000010
#define RGB1_BLUE  0b00000100
#define RGB2_RED   0b00100000
#define RGB2_GREEN 0b10000000
#define RGB2_BLUE  0b01000000

// WiFi
#define WIFI_SSID "Rasp"
#define WIFI_PASSWORD "12345678"
#define WIFI_START_DELAY_MS 3000UL
#define WIFI_CONNECT_TIMEOUT_MS 15000UL
#define WIFI_RETRY_DELAY_MS 10000UL

// Raspberry Pi cible (commande joystick)
// À adapter avec l'IP fixe de ta Raspberry sur le réseau.
#define RPI_IP        "192.168.137.77"
#define RPI_UDP_PORT  5000
// Fréquence d'envoi des infos joystick (en ms)
#define JOY_SEND_INTERVAL_MS 50UL

#endif
