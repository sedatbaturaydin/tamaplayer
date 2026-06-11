#pragma once
#include <stdint.h>

extern int musicIndex;  // su an secili parca

void musicInit();
void musicUpdate();  // loop()'tan cagrilir

void musicNext();
void musicPrev();
void musicPlayPause();

int         musicTrackCount();       // /MP3 altinda bulunan dosya sayisi
const char* musicTrackName(int idx); // 0..count-1; disinda "" donuyor

bool musicSdOk();      // SD mount + dizin okuma basarili mi
bool musicIsPlaying(); // playback aktif mi

// Diagnostic
bool     musicTryLoadFirst();   // index 0'i connecttoFS ile dene, sonuc dondur
uint32_t musicSampleRate();     // audio aktifse sample rate, yoksa 0
uint8_t  musicBitsPerSample();  // 16/24/32 vb.
uint8_t  musicChannels();       // mono/stereo

enum MusicRedraw { MUSIC_REDRAW_NONE, MUSIC_REDRAW_PARTIAL, MUSIC_REDRAW_FULL };

MusicRedraw musicRedrawNeeded();
void        musicClearRedrawFlag();
