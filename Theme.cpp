#include "Theme.h"

// RGB565 values — computed via: ((r&0xF8)<<8) | ((g&0xFC)<<3) | (b>>3)

const Theme THEMES[THEME_COUNT] = {
  {
    // PRIMARY: deep purple background, lilac/lavender accents
    "primary",
    0x1044,   // bg        deep purple-black
    0xB47A,   // accent    soft lilac
    0xDE5E,   // light     pale lavender (text on dark bg)
    0x7B31,   // dim       muted lavender
    0x28A7,   // onAccent  dark purple (text on lilac fill)
  },
  {
    // SECONDARY: rose-lavender background (primary's settings colour), deep purple text
    "secondary",
    0xDD5A,   // bg        pastel rose-lavender
    0x1044,   // accent    deep purple
    0x28A7,   // light     dark purple text on light bg
    0x5990,   // dim       medium purple
    0xDE5E,   // onAccent  pale lavender (text on dark purple fill)
  },
  {
    // ALTERNATIVE: sage green tones (~#9fbda0)
    "alternative",
    0xEFBD,   // bg        very pale green
    0x9DF4,   // accent    sage green
    0x2A45,   // light     dark sage (text on light bg)
    0x7CEF,   // dim       muted sage
    0x2A45,   // onAccent  dark sage (readable on sage-green fill)
  },
};

int currentThemeIndex = 0;
const Theme* activeTheme = &THEMES[0];

void applyTheme(int index) {
  if (index < 0 || index >= THEME_COUNT) return;
  currentThemeIndex = index;
  activeTheme = &THEMES[index];
}
