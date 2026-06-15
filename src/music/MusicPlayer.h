#pragma once
#include <stdint.h>

extern int musicIndex;  // liste imleci (secili parca)

int musicPlayingIndex();  // su an calan track index'i, hicbir track yuklenmediyse -1

void musicInit();
void musicUpdate();  // loop()'tan cagrilir

void musicNext();        // index degisir + track yuklenir + calar (playing context)
void musicPrev();
void musicPlayPause();   // toggle
void musicLoadCurrent(); // mevcut musicIndex'i yukle + cal

// Sadece secimi degistir, calmaya baslama (list context)
void musicSelectNext();
void musicSelectPrev();

int         musicTrackCount();       // /MP3 altinda bulunan dosya sayisi
const char* musicTrackName(int idx); // 0..count-1; disinda "" donuyor

bool musicSdOk();      // SD mount + dizin okuma basarili mi
bool musicIsPlaying(); // playback aktif mi

// ID3 + duration
const char* musicTitle();    // ID3 TIT2 (yoksa dosya adi)
const char* musicArtist();   // ID3 TPE1 (yoksa STR_UNKNOWN)
uint32_t    musicCurrentSec();
uint32_t    musicTotalSec();

// Diagnostic
bool     musicTryLoadFirst();   // index 0'i connecttoFS ile dene, sonuc dondur
uint32_t musicSampleRate();     // audio aktifse sample rate, yoksa 0
uint8_t  musicBitsPerSample();  // 16/24/32 vb.
uint8_t  musicChannels();       // mono/stereo

enum MusicRedraw { MUSIC_REDRAW_NONE, MUSIC_REDRAW_PARTIAL, MUSIC_REDRAW_FULL };

MusicRedraw musicRedrawNeeded();
void        musicClearRedrawFlag();
