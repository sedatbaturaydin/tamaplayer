#pragma once
#include <stdint.h>

struct Track {
  const char* title;
  const char* artist;
  uint16_t    duration; // saniye
};

extern const int TRACK_COUNT;

extern bool     musicPlaying;
extern int      musicIndex;
extern uint16_t musicPos; // saniye cinsinden geçen süre

const Track* musicCurrentTrack();

void musicInit();
void musicUpdate();      // loop()'dan çağrılır — süreyi simüle eder
void musicTogglePlay();
void musicNext();
void musicPrev();        // 3sn üstündeyse başa al, değilse önceki parça

// FULL  → parça değişti, tam ekran yeniden çizilmeli
// PARTIAL → sadece bar/süre/durum güncellenmeli
enum MusicRedraw { MUSIC_REDRAW_NONE, MUSIC_REDRAW_PARTIAL, MUSIC_REDRAW_FULL };

MusicRedraw musicRedrawNeeded();
void        musicClearRedrawFlag();
