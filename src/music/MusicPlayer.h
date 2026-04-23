#pragma once
#include <stdint.h>

extern int musicIndex;  // su an secili parca

void musicInit();
void musicUpdate();  // loop()'tan cagrilir

void musicNext();
void musicPrev();

int         musicTrackCount();       // /MP3 altinda bulunan dosya sayisi
const char* musicTrackName(int idx); // 0..count-1; disinda "" donuyor

bool musicSdOk();  // SD mount + dizin okuma basarili mi

enum MusicRedraw { MUSIC_REDRAW_NONE, MUSIC_REDRAW_PARTIAL, MUSIC_REDRAW_FULL };

MusicRedraw musicRedrawNeeded();
void        musicClearRedrawFlag();
