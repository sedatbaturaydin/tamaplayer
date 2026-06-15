#pragma once

enum SettingsScreen {
  SETTINGS_MAIN,
  SETTINGS_THEME,
  SETTINGS_LANGUAGE,
  SETTINGS_RESET_PET
};

void settingsEnter();   // sayfa acildiginda main screen + index 0
void drawSettingsPage();

void settingsHandleUp();
void settingsHandleDown();
void settingsHandleSelect();
// true: Settings'ten cikilmali (home'a don); false: ic ekran arasinda kaldi
bool settingsHandleBack();
