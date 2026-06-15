#include "Theme.h"

// RGB565 values — computed via: ((r&0xF8)<<8) | ((g&0xFC)<<3) | (b>>3)
// Tum temalar light-mode: acik bg, koyu accent/element

const Theme THEMES[THEME_COUNT] = {
  {
    // LAVENDER — referans music page rengi
    "lavender",
    0xDEDB,   // bg        pale lavender   (~#DACAE3)
    0x39E7,   // accent    deep purple     (~#3D3F3F → koyu mor)
    0x29A4,   // light     dark purple text on light bg
    0x6B53,   // dim       muted purple
    0xDEDB,   // onAccent  pale lavender text on dark fill
  },
  {
    // OLIVE — pale lime bg, dark olive element
    "olive",
    0xE6E7,   // bg        pale yellow-green
    0x4A85,   // accent    dark olive
    0x2A42,   // light     very dark olive text on bg
    0x7BA8,   // dim       muted olive
    0xE6E7,   // onAccent  pale yellow-green on dark fill
  },
  {
    // ORANGE — pale peach bg, dark green element
    "orange",
    0xF513,   // bg        pale peach/orange
    0x2A65,   // accent    dark green
    0x2204,   // light     darker green text
    0x95AB,   // dim       muted brown-orange
    0xF513,   // onAccent  pale peach on dark fill
  },
  {
    // MINT — pale mint bg, forest green element
    "mint",
    0xC6F8,   // bg        pale mint
    0x2B49,   // accent    forest green
    0x1A03,   // light     very dark green text
    0x6429,   // dim       muted sage
    0xC6F8,   // onAccent  pale mint on dark fill
  },
};

int currentThemeIndex = 0;
const Theme* activeTheme = &THEMES[0];

void applyTheme(int index) {
  if (index < 0 || index >= THEME_COUNT) return;
  currentThemeIndex = index;
  activeTheme = &THEMES[index];
}
