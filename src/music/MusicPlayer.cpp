#include "MusicPlayer.h"
#include "../../Config.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "Audio.h"

static SPIClass _spiSD(HSPI);

static char _names[MP3_MAX_TRACKS][MP3_NAME_MAX];
static int  _trackCount = 0;
static bool _sdOk       = false;

static Audio _audio;
static bool  _isPlaying  = false;
static bool  _eofPending = false;

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

static bool loadCurrentTrack() {
  if (_trackCount == 0) return false;
  char path[64];
  snprintf(path, sizeof(path), "%s/%s", MP3_DIR, _names[musicIndex]);
  _audio.stopSong();
  bool ok = _audio.connecttoFS(SD, path);
  _isPlaying = ok;
  if (!ok) Serial.printf("[AUDIO] connecttoFS basarisiz: %s\n", path);
  return ok;
}

void musicInit() {
  _trackCount  = 0;
  _sdOk        = false;
  musicIndex   = 0;
  _isPlaying   = false;
  _eofPending  = false;
  _redraw      = MUSIC_REDRAW_FULL;

  _spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  Serial.printf("[SD] HSPI begin: SCK=%d MISO=%d MOSI=%d CS=%d\n",
                SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  const uint32_t speeds[] = {25000000, 10000000, 4000000, 1000000};
  bool mounted = false;
  for (int i = 0; i < 4; i++) {
    Serial.printf("[SD] mount deneniyor @ %u Hz\n", speeds[i]);
    if (SD.begin(SD_CS, _spiSD, speeds[i])) {
      Serial.printf("[SD] OK @ %u Hz\n", speeds[i]);
      mounted = true;
      break;
    }
    SD.end();
    delay(100);
  }
  if (!mounted) {
    Serial.println("[SD] tum hizlarda basarisiz");
    return;
  }
  _sdOk = true;
  uint8_t cardType = SD.cardType();
  const char* typeStr = "UNKNOWN";
  if (cardType == CARD_NONE) typeStr = "NONE";
  else if (cardType == CARD_MMC) typeStr = "MMC";
  else if (cardType == CARD_SD)  typeStr = "SDSC";
  else if (cardType == CARD_SDHC)typeStr = "SDHC";
  Serial.printf("[SD] mount OK, kart tipi=%s, boyut=%lluMB\n",
                typeStr, SD.cardSize() / (1024ULL * 1024ULL));
  scanMp3Folder();

  _audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  _audio.setVolume(15);  // 0..21
}

void musicUpdate() {
  _audio.loop();
  if (_eofPending) {
    _eofPending = false;
    musicNext();
  }
}

void musicNext() {
  if (_trackCount == 0) return;
  musicIndex = (musicIndex + 1) % _trackCount;
  loadCurrentTrack();
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

void musicPrev() {
  if (_trackCount == 0) return;
  musicIndex = (musicIndex - 1 + _trackCount) % _trackCount;
  loadCurrentTrack();
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

void musicPlayPause() {
  if (_trackCount == 0) return;
  if (!_audio.isRunning()) {
    loadCurrentTrack();
  } else {
    _audio.pauseResume();
    _isPlaying = !_isPlaying;
  }
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

int musicTrackCount() { return _trackCount; }

const char* musicTrackName(int idx) {
  if (idx < 0 || idx >= _trackCount) return "";
  return _names[idx];
}

bool musicSdOk()     { return _sdOk; }
bool musicIsPlaying(){ return _audio.isRunning(); }

bool musicTryLoadFirst() {
  if (_trackCount == 0) return false;
  musicIndex = 0;
  return loadCurrentTrack();
}
uint32_t musicSampleRate()    { return _audio.getSampleRate(); }
uint8_t  musicBitsPerSample() { return _audio.getBitsPerSample(); }
uint8_t  musicChannels()      { return _audio.getChannels(); }

MusicRedraw musicRedrawNeeded()    { return _redraw; }
void        musicClearRedrawFlag() { _redraw = MUSIC_REDRAW_NONE; }

// ESP32-audioI2S extern callback'ler
void audio_eof_mp3(const char* info) {
  (void)info;
  _eofPending = true;
}
void audio_info(const char* info) {
  Serial.print("[audio_info] ");
  Serial.println(info);
}
void audio_id3data(const char* info) {
  Serial.print("[id3] ");
  Serial.println(info);
}
void audio_bitrate(const char* info) {
  Serial.print("[bitrate] ");
  Serial.println(info);
}
