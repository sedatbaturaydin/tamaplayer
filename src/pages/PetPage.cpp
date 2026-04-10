#include "PetPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../pet/PetState.h"
#include <string.h>

static const uint16_t COL_WHITE = 0xFFFF;
static const uint16_t COL_NOSE  = 0xFC10;  // warm pink
static const uint16_t COL_TEAR  = 0xAEDF;  // soft blue

// ── sprite ───────────────────────────────────────────────────────────────────

static void drawEyes(int lx, int rx, int ey, PetMood mood) {
  uint16_t bgCol   = activeTheme->bg;
  uint16_t bodyCol = activeTheme->accent;

  // Sclera
  tft.fillCircle(lx, ey, 5, COL_WHITE);
  tft.fillCircle(rx, ey, 5, COL_WHITE);
  // Iris
  tft.fillCircle(lx, ey, 3, bgCol);
  tft.fillCircle(rx, ey, 3, bgCol);
  // Shine dot
  tft.fillCircle(lx + 1, ey - 1, 1, COL_WHITE);
  tft.fillCircle(rx + 1, ey - 1, 1, COL_WHITE);

  switch (mood) {
    case PET_MOOD_HAPPY:
      // Happy squint: fill upper portion with body colour
      tft.fillRect(lx - 5, ey - 5, 10, 4, bodyCol);
      tft.fillRect(rx - 5, ey - 5, 10, 4, bodyCol);
      break;
    case PET_MOOD_HUNGRY:
      // Heavy droopy lids
      tft.fillRect(lx - 5, ey - 5, 10, 5, bodyCol);
      tft.fillRect(rx - 5, ey - 5, 10, 5, bodyCol);
      break;
    case PET_MOOD_SAD:
      // Droopy inner corners
      tft.fillTriangle(lx - 5, ey - 5,  lx,     ey - 5,  lx,     ey - 1, bodyCol);
      tft.fillTriangle(rx,     ey - 5,  rx + 5, ey - 5,  rx,     ey - 1, bodyCol);
      // Tears
      tft.drawLine(lx,     ey + 4, lx - 2, ey + 12, COL_TEAR);
      tft.drawLine(rx,     ey + 4, rx + 2, ey + 12, COL_TEAR);
      break;
    default:
      break;
  }
}

static void drawMouth(int cx, int my, PetMood mood) {
  switch (mood) {
    case PET_MOOD_HAPPY:
      // ω cat smile
      tft.drawLine(cx,     my,     cx - 5, my + 4, COL_WHITE);
      tft.drawLine(cx,     my,     cx + 5, my + 4, COL_WHITE);
      break;
    case PET_MOOD_SAD:
    case PET_MOOD_HUNGRY:
      // frown
      tft.drawLine(cx - 4, my + 3, cx,     my,     COL_WHITE);
      tft.drawLine(cx,     my,     cx + 4, my + 3, COL_WHITE);
      break;
    default:
      // neutral small w
      tft.drawLine(cx - 4, my,     cx,     my + 2, COL_WHITE);
      tft.drawLine(cx,     my + 2, cx + 4, my,     COL_WHITE);
      break;
  }
}

static void drawCat(int cx, int cy, PetMood mood) {
  uint16_t bodyCol = activeTheme->accent;
  uint16_t bgCol   = activeTheme->bg;
  uint16_t dimCol  = activeTheme->dim;

  // 1. Ears — drawn before head so it covers the bases
  tft.fillTriangle(cx - 22, cy - 12,  cx - 10, cy - 20,  cx - 17, cy - 36,  bodyCol);
  tft.fillTriangle(cx - 19, cy - 15,  cx - 12, cy - 20,  cx - 15, cy - 29,  bgCol);   // inner left
  tft.fillTriangle(cx + 22, cy - 12,  cx + 10, cy - 20,  cx + 17, cy - 36,  bodyCol);
  tft.fillTriangle(cx + 19, cy - 15,  cx + 12, cy - 20,  cx + 15, cy - 29,  bgCol);   // inner right

  // 2. Head
  tft.fillCircle(cx, cy, 22, bodyCol);

  // 3. Eyes
  int eyeY = cy - 7;
  drawEyes(cx - 8, cx + 8, eyeY, mood);

  // 4. Blush cheeks (only when happy)
  if (mood == PET_MOOD_HAPPY) {
    tft.fillCircle(cx - 16, cy + 2, 4, dimCol);
    tft.fillCircle(cx + 16, cy + 2, 4, dimCol);
  }

  // 5. Nose
  tft.fillTriangle(cx - 2, cy + 3,  cx + 2, cy + 3,  cx, cy + 6,  COL_NOSE);

  // 6. Whiskers
  tft.drawLine(cx - 5, cy + 1,  cx - 26, cy - 1,  dimCol);
  tft.drawLine(cx - 5, cy + 4,  cx - 26, cy + 4,  dimCol);
  tft.drawLine(cx - 5, cy + 7,  cx - 26, cy + 9,  dimCol);
  tft.drawLine(cx + 5, cy + 1,  cx + 26, cy - 1,  dimCol);
  tft.drawLine(cx + 5, cy + 4,  cx + 26, cy + 4,  dimCol);
  tft.drawLine(cx + 5, cy + 7,  cx + 26, cy + 9,  dimCol);

  // 7. Mouth
  drawMouth(cx, cy + 8, mood);

  // 8. Extra mood cues
  if (mood == PET_MOOD_HUNGRY) {
    tft.setTextSize(1);
    tft.setTextColor(activeTheme->light);
    tft.setCursor(cx + 24, cy - 32);
    tft.print("!");
  }
}

// ── stat bar ─────────────────────────────────────────────────────────────────

static void drawStatBar(int x, int y, const char* label, uint8_t value, uint8_t maxVal) {
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->dim);
  tft.setCursor(x, y);
  tft.print(label);

  int barX  = x + 24;
  int barW  = 88;
  int fillW = (value * (barW - 2)) / maxVal;

  tft.drawRect(barX, y - 1, barW, 8, activeTheme->dim);
  if (fillW > 0) {
    tft.fillRect(barX + 1, y, fillW, 6, activeTheme->accent);
  }
}

// ── public ───────────────────────────────────────────────────────────────────

void drawPetPage() {
  tft.fillScreen(activeTheme->bg);

  // Title
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light);
  tft.setCursor(52, 10);
  tft.println("PET");
  tft.drawFastHLine(10, 22, 108, activeTheme->accent);

  PetMood mood = petGetMood();
  drawCat(64, 70, mood);

  // Mood label
  const char* moodText;
  switch (mood) {
    case PET_MOOD_HAPPY:   moodText = "happy!";    break;
    case PET_MOOD_HUNGRY:  moodText = "hungry..."; break;
    case PET_MOOD_SAD:     moodText = "sad...";    break;
    default:               moodText = "( - _ - )"; break;
  }
  int mw = strlen(moodText) * 6;
  tft.setTextColor(activeTheme->dim);
  tft.setCursor((128 - mw) / 2, 102);
  tft.println(moodText);

  // Stat bars
  drawStatBar(8, 118, "hun", 10 - petStats.hunger,  10);
  drawStatBar(8, 132, "hpy", petStats.happiness,     10);

  // Hint
  tft.setTextColor(activeTheme->dim);
  tft.setCursor(20, 150);
  tft.println("press: interact");
}
