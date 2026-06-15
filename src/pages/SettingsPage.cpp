#include "SettingsPage.h"
#include "../display/DisplayManager.h"
#include "../display/Theme.h"
#include "../display/StatusBar.h"
#include "../i18n/Strings.h"
#include "../pet/PetState.h"
#include <string.h>

static SettingsScreen _screen = SETTINGS_MAIN;
static int _index = 0;

// Ana menude 3 satir
static const StringId MAIN_ITEMS[] = {
  STR_THEME, STR_LANGUAGE, STR_RESET_PET
};
static const int MAIN_COUNT = sizeof(MAIN_ITEMS) / sizeof(MAIN_ITEMS[0]);

static int currentCount() {
  switch (_screen) {
    case SETTINGS_MAIN:      return MAIN_COUNT;
    case SETTINGS_THEME:     return THEME_COUNT;
    case SETTINGS_LANGUAGE:  return LANG_COUNT;
    case SETTINGS_RESET_PET: return 1;
  }
  return 1;
}

void settingsEnter() {
  _screen = SETTINGS_MAIN;
  _index  = 0;
}

// ── render helpers ───────────────────────────────────────────────────────────

static void drawTitle(const char* title) {
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  int tw = (int)strlen(title) * 6;
  tft.setCursor((SCREEN_W - tw) / 2, PH(18));
  tft.println(title);
  tft.drawFastHLine(PW(10), PH(28), SCREEN_W - PW(20), activeTheme->accent);
}

static void drawListRow(int y, const char* label, bool selected, bool active) {
  if (selected) {
    tft.fillRoundRect(PW(8), y - 3, SCREEN_W - PW(16), PH(16), 4, activeTheme->accent);
    tft.setTextColor(activeTheme->onAccent);
    tft.setCursor(PW(14), y);
    tft.print("> ");
    tft.print(label);
    if (active) tft.print(" *");
  } else {
    tft.setTextColor(activeTheme->dim);
    tft.setCursor(PW(22), y);
    tft.print(label);
    if (active) {
      tft.setTextColor(activeTheme->accent);
      tft.print(" *");
    }
  }
}

static void drawMain() {
  drawTitle(T(STR_SETTINGS));
  for (int i = 0; i < MAIN_COUNT; i++) {
    int y = PH(48) + i * PH(22);
    drawListRow(y, T(MAIN_ITEMS[i]), i == _index, false);
  }
}

static void drawThemeSel() {
  drawTitle(T(STR_THEME));
  for (int i = 0; i < THEME_COUNT; i++) {
    int y = PH(44) + i * PH(20);
    drawListRow(y, THEMES[i].name, i == _index, i == currentThemeIndex);
  }
}

static void drawLangSel() {
  drawTitle(T(STR_LANGUAGE));
  static const StringId LANGS[LANG_COUNT] = { STR_LANG_EN, STR_LANG_TR };
  for (int i = 0; i < LANG_COUNT; i++) {
    int y = PH(48) + i * PH(22);
    drawListRow(y, T(LANGS[i]), i == _index, i == currentLanguage);
  }
}

static void drawResetConfirm() {
  drawTitle(T(STR_RESET_PET));

  const char* q = T(STR_RESET_CONFIRM);
  int qw = (int)strlen(q) * 6;
  tft.setTextColor(activeTheme->light, activeTheme->bg);
  tft.setCursor((SCREEN_W - qw) / 2, SCREEN_H / 2 - 6);
  tft.print(q);

  // hint: OK uygula, escape iptal
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  const char* hint = "ok / esc";
  int hw = (int)strlen(hint) * 6;
  tft.setCursor((SCREEN_W - hw) / 2, SCREEN_H - 16);
  tft.print(hint);
}

// ── public ───────────────────────────────────────────────────────────────────

void drawSettingsPage() {
  tft.fillScreen(activeTheme->bg);
  drawStatusBar();

  switch (_screen) {
    case SETTINGS_MAIN:      drawMain(); break;
    case SETTINGS_THEME:     drawThemeSel(); break;
    case SETTINGS_LANGUAGE:  drawLangSel(); break;
    case SETTINGS_RESET_PET: drawResetConfirm(); break;
  }
}

void settingsHandleUp() {
  int n = currentCount();
  if (n <= 1) return;
  _index = (_index - 1 + n) % n;
  drawSettingsPage();
}

void settingsHandleDown() {
  int n = currentCount();
  if (n <= 1) return;
  _index = (_index + 1) % n;
  drawSettingsPage();
}

void settingsHandleSelect() {
  switch (_screen) {
    case SETTINGS_MAIN:
      switch (_index) {
        case 0: _screen = SETTINGS_THEME;    _index = currentThemeIndex; break;
        case 1: _screen = SETTINGS_LANGUAGE; _index = currentLanguage;   break;
        case 2: _screen = SETTINGS_RESET_PET; _index = 0;                break;
      }
      break;
    case SETTINGS_THEME:
      applyTheme(_index);
      _screen = SETTINGS_MAIN;
      _index = 0;
      break;
    case SETTINGS_LANGUAGE:
      applyLanguage(_index);
      _screen = SETTINGS_MAIN;
      _index = 1;  // sectigi item'da kalsin
      break;
    case SETTINGS_RESET_PET:
      petInit();
      _screen = SETTINGS_MAIN;
      _index = 2;
      break;
  }
  drawSettingsPage();
}

bool settingsHandleBack() {
  if (_screen == SETTINGS_MAIN) {
    return true;  // home'a don
  }
  _screen = SETTINGS_MAIN;
  _index = 0;
  drawSettingsPage();
  return false;
}
