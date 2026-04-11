#pragma once

// TFT pinleri
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    16
#define TFT_MOSI  17
#define TFT_SCLK  18

// Butonlar
#define BTN_UP     35
#define BTN_SELECT 36
#define BTN_DOWN   37

// ── Ekran yönü ───────────────────────────────────────────────────────────────
// 0 = dikey  (128×160)   
// 1 = yatay  (160×128)
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
