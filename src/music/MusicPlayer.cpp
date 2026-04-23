#include "MusicPlayer.h"
#include "../../Config.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

static SPIClass _spiSD(HSPI);

static char _names[MP3_MAX_TRACKS][MP3_NAME_MAX];
static int  _trackCount = 0;
static bool _sdOk       = false;

int musicIndex = 0;

static MusicRedraw _redraw = MUSIC_REDRAW_FULL;

static void setRedraw(MusicRedraw level) {
  if (level > _redraw) _redraw = level;
}

static bool isMp3(const char* name) {
  size_t n = strlen(name);
  if (n < 5) return false;
  const char* ext = name + n - 4;
  return (ext[0] == '.') &&
         (ext[1] == 'm' || ext[1] == 'M') &&
         (ext[2] == 'p' || ext[2] == 'P') &&
         (ext[3] == '3');
}

static void scanMp3Folder() {
  _trackCount = 0;
  File dir = SD.open(MP3_DIR);
  if (!dir || !dir.isDirectory()) {
    Serial.println("[SD] /MP3 dizini yok");
    if (dir) dir.close();
    return;
  }

  File f = dir.openNextFile();
  while (f && _trackCount < MP3_MAX_TRACKS) {
    if (!f.isDirectory()) {
      const char* nm = f.name();
      // FS bazen tam yol verir; dosya adini ayikla
      const char* slash = strrchr(nm, '/');
      if (slash) nm = slash + 1;
      if (isMp3(nm)) {
        strncpy(_names[_trackCount], nm, MP3_NAME_MAX - 1);
        _names[_trackCount][MP3_NAME_MAX - 1] = '\0';
        _trackCount++;
      }
    }
    f.close();
    f = dir.openNextFile();
  }
  dir.close();

  Serial.print("[SD] ");
  Serial.print(_trackCount);
  Serial.println(" mp3 bulundu");
  for (int i = 0; i < _trackCount; i++) {
    Serial.print("  "); Serial.println(_names[i]);
  }
}

void musicInit() {
  _trackCount = 0;
  _sdOk       = false;
  musicIndex  = 0;
  _redraw     = MUSIC_REDRAW_FULL;

  _spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, _spiSD)) {
    Serial.println("[SD] mount basarisiz");
    return;
  }
  _sdOk = true;
  Serial.println("[SD] mount OK");
  scanMp3Folder();
}

void musicUpdate() {
  // SD-only listeleme modunda surekli is yok
}

void musicNext() {
  if (_trackCount == 0) return;
  musicIndex = (musicIndex + 1) % _trackCount;
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

void musicPrev() {
  if (_trackCount == 0) return;
  musicIndex = (musicIndex - 1 + _trackCount) % _trackCount;
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

int musicTrackCount() { return _trackCount; }

const char* musicTrackName(int idx) {
  if (idx < 0 || idx >= _trackCount) return "";
  return _names[idx];
}

bool musicSdOk() { return _sdOk; }

MusicRedraw musicRedrawNeeded()    { return _redraw; }
void        musicClearRedrawFlag() { _redraw = MUSIC_REDRAW_NONE; }
