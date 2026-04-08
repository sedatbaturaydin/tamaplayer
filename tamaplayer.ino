#include "Config.h"
#include "AppState.h"
#include "DisplayManager.h"
#include "HomePage.h"
#include "Pages.h"

AppState currentPage = PAGE_HOME;
int homeSelectedIndex = 0;

bool lastReading = HIGH;
bool stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long pressStartTime = 0;
bool longPressHandled = false;

void renderCurrentPage() {
  if (currentPage == PAGE_HOME) {
    drawHomePage(homeSelectedIndex);
  } else {
    drawPage(currentPage);
  }
}

AppState menuIndexToPage(int index) {
  switch (index) {
    case 0: return PAGE_PET;
    case 1: return PAGE_MUSIC;
    case 2: return PAGE_STATUS;
    case 3: return PAGE_SETTINGS;
    default: return PAGE_HOME;
  }
}

void handleShortPress() {
  if (currentPage == PAGE_HOME) {
    homeSelectedIndex++;
    if (homeSelectedIndex >= getHomeMenuCount()) {
      homeSelectedIndex = 0;
    }
    drawHomePage(homeSelectedIndex);
  }
}

void handleLongPress() {
  if (currentPage == PAGE_HOME) {
    currentPage = menuIndexToPage(homeSelectedIndex);
  } else {
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

  initDisplay();
  renderCurrentPage();
}

void loop() {
  updateButton();
}