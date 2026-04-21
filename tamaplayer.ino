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

AppState currentPage = PAGE_HOME;
int homeSelectedIndex = 0;
int settingsSelectedTheme = 0;

// --- Buton debounce yardımcısı ---

struct BtnState {
  bool lastReading  = HIGH;
  bool stable       = HIGH;
  unsigned long debounceAt = 0;
  unsigned long pressedAt  = 0;
  bool longHandled  = false;
};

BtnState btnUp, btnSelect, btnDown;

// Her frame çağrılır. shortPress / longPress flag'lerini doldurur.
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

// --- Sayfa render ---

void renderCurrentPage() {
  if (currentPage == PAGE_HOME) {
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_PET) {
    drawPetPage();
    petClearRedrawFlag();
  } else if (currentPage == PAGE_SETTINGS) {
    drawSettingsPage(settingsSelectedTheme);
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

// --- Buton olayları ---

void handleUp() {
  if (currentPage == PAGE_HOME) {
    homeSelectedIndex = (homeSelectedIndex - 1 + getHomeMenuCount()) % getHomeMenuCount();
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_SETTINGS) {
    settingsSelectedTheme = (settingsSelectedTheme - 1 + THEME_COUNT) % THEME_COUNT;
    drawSettingsPage(settingsSelectedTheme);
  } else if (currentPage == PAGE_MUSIC) {
    musicPrev();
  }
}

void handleDown() {
  if (currentPage == PAGE_HOME) {
    homeSelectedIndex = (homeSelectedIndex + 1) % getHomeMenuCount();
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_SETTINGS) {
    settingsSelectedTheme = (settingsSelectedTheme + 1) % THEME_COUNT;
    drawSettingsPage(settingsSelectedTheme);
  } else if (currentPage == PAGE_MUSIC) {
    musicNext();
  }
}

void handleSelect() {
  if (currentPage == PAGE_HOME) {
    AppState next = menuIndexToPage(homeSelectedIndex);
    if (next == PAGE_SETTINGS) settingsSelectedTheme = currentThemeIndex;
    currentPage = next;
    renderCurrentPage();
  } else if (currentPage == PAGE_PET) {
    petHandleAction(PET_ACTION_INTERACT);
  } else if (currentPage == PAGE_MUSIC) {
    musicTogglePlay();
  }
}

void handleBack() {
  if (currentPage == PAGE_SETTINGS) {
    applyTheme(settingsSelectedTheme);
  }
  if (currentPage != PAGE_HOME) {
    currentPage = PAGE_HOME;
    renderCurrentPage();
  }
}

// --- Arduino setup / loop ---

void setup() {
  Serial.begin(115200);
  pinMode(BTN_UP,     INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_DOWN,   INPUT_PULLUP);

  initDisplay();
  renderCurrentPage();
  petInit();
  musicInit();
}

void loop() {
  bool shortPress, longPress;

  updateBtn(btnUp, BTN_UP, shortPress, longPress);
  if (shortPress || longPress) handleUp();

  updateBtn(btnDown, BTN_DOWN, shortPress, longPress);
  if (shortPress || longPress) handleDown();

  updateBtn(btnSelect, BTN_SELECT, shortPress, longPress);
  if (shortPress) handleSelect();
  if (longPress)  handleBack();

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
  }
}
