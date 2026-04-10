#pragma once
#include <stdint.h>

struct Theme {
  const char* name;
  uint16_t bg;       // background for all screens
  uint16_t accent;   // highlight fills, lines, breadcrumb
  uint16_t light;    // primary text on bg
  uint16_t dim;      // secondary / unselected text on bg
  uint16_t onAccent; // text on top of accent-colored fills
};

#define THEME_COUNT 3

extern const Theme THEMES[THEME_COUNT];
extern const Theme* activeTheme;
extern int currentThemeIndex;

void applyTheme(int index);
