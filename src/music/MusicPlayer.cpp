#include "MusicPlayer.h"
#include <Arduino.h>

static const Track TRACKS[] = {
  { "Basketball Shoes",  "Black Country, New Road", 13 },
  { "Glory of the Snow", "Clairo", 15 },
  { "Cool About It", "Boygenius", 12 },
  { "Au Pays du Cocaine", "Geese", 11 },
};

const int TRACK_COUNT = sizeof(TRACKS) / sizeof(TRACKS[0]);

bool     musicPlaying = false;
int      musicIndex   = 0;
uint16_t musicPos     = 0;

static MusicRedraw _redraw     = MUSIC_REDRAW_FULL;
static uint32_t    _lastTickMs = 0;

const Track* musicCurrentTrack() {
  return &TRACKS[musicIndex];
}

static void setRedraw(MusicRedraw level) {
  if (level > _redraw) _redraw = level; // daha yüksek öncelik kazanır
}

void musicInit() {
  musicPlaying = false;
  musicIndex   = 0;
  musicPos     = 0;
  _redraw      = MUSIC_REDRAW_FULL;
  _lastTickMs  = millis();
}

void musicUpdate() {
  if (!musicPlaying) return;

  uint32_t now = millis();
  if (now - _lastTickMs >= 1000) {
    _lastTickMs = now;
    musicPos++;
    if (musicPos >= musicCurrentTrack()->duration) {
      musicIndex = (musicIndex + 1) % TRACK_COUNT;
      musicPos   = 0;
      setRedraw(MUSIC_REDRAW_FULL);    // parça değişti
    } else {
      setRedraw(MUSIC_REDRAW_PARTIAL); // sadece bar/süre
    }
  }
}

void musicTogglePlay() {
  musicPlaying = !musicPlaying;
  if (musicPlaying) _lastTickMs = millis();
  setRedraw(MUSIC_REDRAW_PARTIAL); // sadece durum satırı
}

void musicNext() {
  musicIndex = (musicIndex + 1) % TRACK_COUNT;
  musicPos   = 0;
  setRedraw(MUSIC_REDRAW_FULL);
}

void musicPrev() {
  if (musicPos > 3) {
    musicPos = 0;
  } else {
    musicIndex = (musicIndex - 1 + TRACK_COUNT) % TRACK_COUNT;
    musicPos   = 0;
  }
  setRedraw(MUSIC_REDRAW_FULL);
}

MusicRedraw musicRedrawNeeded()  { return _redraw; }
void        musicClearRedrawFlag() { _redraw = MUSIC_REDRAW_NONE; }
