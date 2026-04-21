#pragma once
#include <stdint.h>

extern bool musicPlaying;
extern int  musicIndex;

void musicInit();
void musicUpdate();      // loop()'tan cagrilir - DFPlayer event pompasi
void musicTogglePlay();
void musicNext();
void musicPrev();

int  musicTrackCount();  // SD'deki toplam dosya sayisi (0 = SD yok / DFP hatasi)

// Diagnostic: DFPlayer'in gercekten ne durumda oldugu
int  musicActualState();    // 0=stop, 1=play, 2=pause, -1=bilinmiyor
int  musicActualFileNum();  // DFPlayer'in bildirdigi calmakta olan dosya no

enum MusicRedraw { MUSIC_REDRAW_NONE, MUSIC_REDRAW_PARTIAL, MUSIC_REDRAW_FULL };

MusicRedraw musicRedrawNeeded();
void        musicClearRedrawFlag();
