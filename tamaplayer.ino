#include "Config.h"
#include "AppState.h"
#include "src/display/Theme.h"
#include "src/display/DisplayManager.h"
#include "src/pages/HomePage.h"
#include "src/pages/Pages.h"
#include "src/pages/PetPage.h"
#include "src/pet/PetState.h"
#include "src/pages/SettingsPage.h"

AppState currentPage = PAGE_HOME;
int homeSelectedIndex = 0;
int settingsSelectedTheme = 0;

bool lastReading = HIGH;
bool stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long pressStartTime = 0;
bool longPressHandled = false;

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

void handleShortPress() {
  if (currentPage == PAGE_HOME) {
    homeSelectedIndex = (homeSelectedIndex + 1) % getHomeMenuCount();
    drawHomePage(homeSelectedIndex);
  } else if (currentPage == PAGE_PET) {
    petHandleAction(PET_ACTION_INTERACT);
    // redraw handled in loop()
  } else if (currentPage == PAGE_SETTINGS) {
    settingsSelectedTheme = (settingsSelectedTheme + 1) % THEME_COUNT;
    drawSettingsPage(settingsSelectedTheme);
  }
}

void handleLongPress() {
  if (currentPage == PAGE_HOME) {
    AppState next = menuIndexToPage(homeSelectedIndex);
    if (next == PAGE_SETTINGS) {
      settingsSelectedTheme = currentThemeIndex;
    }
    currentPage = next;
  } else {
    if (currentPage == PAGE_SETTINGS) {
      applyTheme(settingsSelectedTheme);
    }
    currentPage = PAGE_HOME;
  }

  renderCurrentPage();
}

void updateButton() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == LOW) {
        pressStartTime = millis();
        longPressHandled = false;
      } else {
        if (!longPressHandled) {
          handleShortPress();
        }
      }
    }
  }

  if (stableButtonState == LOW && !longPressHandled) {
    if (millis() - pressStartTime >= LONG_PRESS_TIME) {
      longPressHandled = true;
      handleLongPress();
    }
  }

  lastReading = reading;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  petInit();
  initDisplay();
  renderCurrentPage();
}

void loop() {
  updateButton();

  // Pet stats decay regardless of which page is visible
  petUpdate();

  // Redraw pet page if stats changed while it's open
  if (currentPage == PAGE_PET && petNeedsRedraw()) {
    drawPetPage();
    petClearRedrawFlag();
  }
}
