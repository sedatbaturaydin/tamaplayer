#pragma once

enum Lang {
  LANG_EN = 0,
  LANG_TR = 1
};

#define LANG_COUNT 2

enum StringId {
  STR_HOME,
  STR_PET,
  STR_MUSIC,
  STR_SETTINGS,
  STR_THEME,
  STR_LANGUAGE,
  STR_RESET_PET,
  STR_RESET_CONFIRM,
  STR_LOADING,
  STR_NO_SD,
  STR_NO_MP3,
  STR_HAPPY,
  STR_HUNGRY,
  STR_SAD,
  STR_NEUTRAL,
  STR_UP_DOWN_CYCLE,
  STR_HOLD_APPLY,
  STR_PRESS_INTERACT,
  STR_LANG_EN,
  STR_LANG_TR,
  STR_UNKNOWN,
  STR_COUNT
};

extern int currentLanguage;

void applyLanguage(int lang);
const char* T(StringId id);
