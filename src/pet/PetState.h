#pragma once
#include <Arduino.h>

// Milliseconds between stat decay ticks — lower = faster, good for testing
#define PET_TICK_MS 60000UL

// Extend this enum when new buttons arrive (FEED, PLAY, MEDICINE, etc.)
enum PetAction {
  PET_ACTION_INTERACT   // single-button smart action for now
};

enum PetMood {
  PET_MOOD_HAPPY,
  PET_MOOD_NEUTRAL,
  PET_MOOD_HUNGRY,
  PET_MOOD_SAD
};

struct PetStats {
  uint8_t hunger;     // 0 = full, 10 = starving
  uint8_t happiness;  // 0 = sad, 10 = happy
};

extern PetStats petStats;

void petInit();
void petUpdate();                     // call every loop()
void petHandleAction(PetAction action);
PetMood petGetMood();
bool petNeedsRedraw();
void petClearRedrawFlag();
