#pragma once

// TFT ST7735S (FSPI / default SPI)
#define TFT_CS    40
#define TFT_DC    41
#define TFT_RST   42
#define TFT_SCK    1
#define TFT_MOSI   2

// SD card (HSPI - ayri bus)
#define SD_SCK    12
#define SD_MISO   13
#define SD_MOSI   11
#define SD_CS     10

// I2S (PCM5102A DAC)
#define I2S_DOUT   7   // DIN  — strap'ten tasindi (eski: 45)
#define I2S_BCLK  15   // BCK  — strap'ten tasindi (eski: 46)
#define I2S_LRC   47   // LCK  — guvenli, kaldi

// Butonlar (INPUT_PULLUP, active LOW)
#define BTN_UP      5   // geri
#define BTN_DOWN    6   // ileri
#define BTN_SELECT 17   // ok
#define BTN_BACK   18   // escape

// Ekran yonu
// 0 = dikey  (128x160)
// 1 = yatay  (160x128)
// 2 = dikey ters
// 3 = yatay ters
#define SCREEN_ROTATION 0

#if (SCREEN_ROTATION % 2) == 0
  #define SCREEN_W 128
  #define SCREEN_H 160
#else
  #define SCREEN_W 160
  #define SCREEN_H 128
#endif

#define PH(y) ((y) * SCREEN_H / 160)
#define PW(x) ((x) * SCREEN_W / 128)

// Buton timing
#define DEBOUNCE_DELAY 50
#define LONG_PRESS_TIME 700

// MP3 klasoru
#define MP3_DIR        "/MP3"
#define MP3_MAX_TRACKS 32
#define MP3_NAME_MAX   32
