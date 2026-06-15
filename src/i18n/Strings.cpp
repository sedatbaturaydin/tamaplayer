#include "Strings.h"

int currentLanguage = LANG_EN;

// Adafruit_GFX default font ASCII-only, Turkce karakterleri ASCII karsiliklariyla yazdik
// (s/g/c/u/o/i). Custom font eklenirse degistirilir.

static const char* const TABLE[STR_COUNT][LANG_COUNT] = {
  /* STR_HOME            */ { "home",            "ana sayfa" },
  /* STR_PET             */ { "pet",             "evcil" },
  /* STR_MUSIC           */ { "music",           "muzik" },
  /* STR_SETTINGS        */ { "settings",        "ayarlar" },
  /* STR_THEME           */ { "theme",           "tema" },
  /* STR_LANGUAGE        */ { "language",        "dil" },
  /* STR_RESET_PET       */ { "reset pet",       "evcili sifirla" },
  /* STR_RESET_CONFIRM   */ { "reset stats?",    "sifirla?" },
  /* STR_LOADING         */ { "loading",         "yukleniyor" },
  /* STR_NO_SD           */ { "no sd card",      "sd yok" },
  /* STR_NO_MP3          */ { "no mp3 files",    "mp3 yok" },
  /* STR_HAPPY           */ { "happy!",          "mutlu!" },
  /* STR_HUNGRY          */ { "hungry...",       "ac..." },
  /* STR_SAD             */ { "sad...",          "uzgun..." },
  /* STR_NEUTRAL         */ { "( - _ - )",       "( - _ - )" },
  /* STR_UP_DOWN_CYCLE   */ { "up/down: cycle",  "yon: degistir" },
  /* STR_HOLD_APPLY      */ { "hold: apply",     "basili: uygula" },
  /* STR_PRESS_INTERACT  */ { "press: interact", "bas: etkilesim" },
  /* STR_LANG_EN         */ { "en",              "en" },
  /* STR_LANG_TR         */ { "tr",              "tr" },
  /* STR_UNKNOWN         */ { "unknown",         "bilinmiyor" },
};

void applyLanguage(int lang) {
  if (lang < 0 || lang >= LANG_COUNT) return;
  currentLanguage = lang;
}

const char* T(StringId id) {
  if (id < 0 || id >= STR_COUNT) return "";
  return TABLE[id][currentLanguage];
}
