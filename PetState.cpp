#include "PetState.h"

PetStats petStats = { 0, 8 };  // start well-fed and happy

static unsigned long lastTick   = 0;
static bool          redrawFlag = false;

void petInit() {
  petStats.hunger    = 0;
  petStats.happiness = 8;
  lastTick   = millis();
  redrawFlag = true;
}

void petUpdate() {
  unsigned long now = millis();
  if (now - lastTick < PET_TICK_MS) return;
  lastTick = now;

  if (petStats.hunger    < 10) petStats.hunger++;
  if (petStats.happiness >  0) petStats.happiness--;

  redrawFlag = true;
}

void petHandleAction(PetAction action) {
  switch (action) {
    case PET_ACTION_INTERACT:
      // Smart single-button: feed if hungry, play otherwise.
      // When more buttons arrive, split into PET_ACTION_FEED / PET_ACTION_PLAY.
      if (petStats.hunger >= 5) {
        petStats.hunger = (petStats.hunger > 3) ? petStats.hunger - 3 : 0;
      } else {
        petStats.happiness = (petStats.happiness < 8) ? petStats.happiness + 2 : 10;
      }
      redrawFlag = true;
      break;
  }
}

PetMood petGetMood() {
  if (petStats.hunger    >= 8) return PET_MOOD_HUNGRY;
  if (petStats.happiness <= 2) return PET_MOOD_SAD;
  if (petStats.happiness >= 7) return PET_MOOD_HAPPY;
  return PET_MOOD_NEUTRAL;
}

bool petNeedsRedraw()     { return redrawFlag; }
void petClearRedrawFlag() { redrawFlag = false; }
