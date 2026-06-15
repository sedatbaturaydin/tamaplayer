#include "Config.h"
#include "AppState.h"
#include "src/display/Theme.h"
#include "src/display/DisplayManager.h"
#include "src/pages/HomePage.h"
#include "src/pages/Pages.h"
#include "src/pages/PetPage.h"
#include "src/pet/PetState.h"
#include "src/pages/SettingsPage.h"
#include "src/pages/MusicPage.h"
#include "src/music/MusicPlayer.h"
#include "src/i18n/Strings.h"

AppState currentPage = PAGE_HOME;
int homeSelectedIndex = 0;

struct BtnState {
  bool lastReading  = HIGH;
  bool stable       = HIGH;
  unsigned long debounceAt = 0;
  unsigned long pressedAt  = 0;
  bool longHandled  = false;
};

BtnState btnUp, btnSelect, btnDown, btnBack;

void updateBtn(BtnState &b, uint8_t pin, bool &shortPress, bool &longPress) {
  shortPress = false;
  longPress  = false;

  bool reading = digitalRead(pin);

  if (reading != b.lastReading) b.debounceAt = millis();

  if ((millis() - b.debounceAt) > DEBOUNCE_DELAY) {
    if (reading != b.stable) {
      b.stable = reading;
      if (b.stable == LOW) {
        b.pressedAt  = millis();
        b.longHandled = false;
      } else {
        if (!b.longHandled) shortPress = true;
      }
    }
  }

  if (b.stable == LOW && !b.longHandled) {
    if (millis() - b.pressedAt >= LONG_PRESS_TIME) {
      b.longHandled = true;
      longPress = true;
    }
  }

  b.lastReading = reading;
}

void renderCurrentPage() {
  if (currentPage == PAGE_HOME) {
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_PET) {
    drawPetPage();
    petClearRedrawFlag();
  } else if (currentPage == PAGE_SETTINGS) {
    drawSettingsPage();
  } else {
    drawPage(currentPage);
  }
}

AppState menuIndexToPage(int index) {
  switch (index) {
    case 0: return PAGE_PET;
    case 1: return PAGE_MUSIC;
    case 2: return PAGE_SETTINGS;
    default: return PAGE_HOME;
  }
}

void handleUp() {
  if (currentPage == PAGE_HOME) {
    homeSelectedIndex = (homeSelectedIndex - 1 + getHomeMenuCount()) % getHomeMenuCount();
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_SETTINGS) {
    settingsHandleUp();
  } else if (currentPage == PAGE_MUSIC) {
    if (musicPageGetView() == MUSIC_VIEW_LIST) {
      musicSelectPrev();   // sadece secim degisir
    } else {
      musicPrev();         // playing: yeni track calsin
    }
  }
}

void handleDown() {
  if (currentPage == PAGE_HOME) {
    homeSelectedIndex = (homeSelectedIndex + 1) % getHomeMenuCount();
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_SETTINGS) {
    settingsHandleDown();
  } else if (currentPage == PAGE_MUSIC) {
    if (musicPageGetView() == MUSIC_VIEW_LIST) {
      musicSelectNext();
    } else {
      musicNext();
    }
  }
}

void handleSelect() {
  if (currentPage == PAGE_HOME) {
    AppState next = menuIndexToPage(homeSelectedIndex);
    if (next == PAGE_SETTINGS) settingsEnter();
    if (next == PAGE_MUSIC) {
      // Calan track varsa list imlecini ona hizala
      int p = musicPlayingIndex();
      if (p >= 0) musicIndex = p;
      musicPageSetView(musicIsPlaying() ? MUSIC_VIEW_PLAYING : MUSIC_VIEW_LIST);
    }
    currentPage = next;
    renderCurrentPage();
  } else if (currentPage == PAGE_PET) {
    petHandleAction(PET_ACTION_INTERACT);
  } else if (currentPage == PAGE_MUSIC) {
    if (musicPageGetView() == MUSIC_VIEW_LIST) {
      musicLoadCurrent();              // secili track'i yukle ve cal
      musicPageSetView(MUSIC_VIEW_PLAYING);
      drawMusicPage();
    } else {
      musicPlayPause();                // playing: pause/resume
    }
  } else if (currentPage == PAGE_SETTINGS) {
    settingsHandleSelect();
  }
}

void handleBack() {
  if (currentPage == PAGE_SETTINGS) {
    if (!settingsHandleBack()) return;  // ic ekran arasinda kaldi
  } else if (currentPage == PAGE_MUSIC) {
    if (musicPageGetView() == MUSIC_VIEW_PLAYING) {
      musicPageSetView(MUSIC_VIEW_LIST);
      drawMusicPage();
      return;
    }
  }
  if (currentPage != PAGE_HOME) {
    currentPage = PAGE_HOME;
    renderCurrentPage();
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  Serial.println("=== TAMAPLAYER BOOT ===");
  Serial.println("Build: " __DATE__ " " __TIME__);

  pinMode(BTN_UP,     INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_DOWN,   INPUT_PULLUP);
  pinMode(BTN_BACK,   INPUT_PULLUP);

  initDisplay();

  // Boot splash
  tft.fillScreen(activeTheme->bg);
  const char* brand = "tamaplayer";
  const char* loading = T(STR_LOADING);
  int bw = (int)strlen(brand) * 6 * 2;
  int lw = (int)strlen(loading) * 6;
  tft.setTextSize(2);
  tft.setTextColor(activeTheme->accent, activeTheme->bg);
  tft.setCursor((SCREEN_W - bw) / 2, SCREEN_H / 2 - 14);
  tft.print(brand);
  tft.setTextSize(1);
  tft.setTextColor(activeTheme->dim, activeTheme->bg);
  tft.setCursor((SCREEN_W - lw) / 2, SCREEN_H / 2 + 12);
  tft.print(loading);

  unsigned long splashStart = millis();
  musicInit();
  unsigned long elapsed = millis() - splashStart;
  if (elapsed < 1500) delay(1500 - elapsed);

  renderCurrentPage();
  petInit();

  Serial.println("[SETUP] tamamlandi");
}

void loop() {
  bool shortPress, longPress;

  updateBtn(btnUp, BTN_UP, shortPress, longPress);
  if (shortPress || longPress) handleUp();

  updateBtn(btnDown, BTN_DOWN, shortPress, longPress);
  if (shortPress || longPress) handleDown();

  updateBtn(btnSelect, BTN_SELECT, shortPress, longPress);
  if (shortPress) handleSelect();

  updateBtn(btnBack, BTN_BACK, shortPress, longPress);
  if (shortPress || longPress) handleBack();

  petUpdate();
  musicUpdate();

  if (currentPage == PAGE_PET && petNeedsRedraw()) {
    drawPetPage();
    petClearRedrawFlag();
  }

  if (currentPage == PAGE_MUSIC) {
    MusicRedraw r = musicRedrawNeeded();
    if (r == MUSIC_REDRAW_FULL) {
      drawMusicPage();
      musicClearRedrawFlag();
    } else if (r == MUSIC_REDRAW_PARTIAL) {
      drawMusicPagePartial();
      musicClearRedrawFlag();
    }

    // Playing view: progress + zamani periyodik tazele
    static unsigned long lastTick = 0;
    if (musicPageGetView() == MUSIC_VIEW_PLAYING && millis() - lastTick > 500) {
      lastTick = millis();
      drawMusicPagePartial();
    }
  }
}
