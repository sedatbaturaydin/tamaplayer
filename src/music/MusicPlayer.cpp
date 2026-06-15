#include "MusicPlayer.h"
#include "../../Config.h"
#include "../i18n/Strings.h"
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
static int   _playingIndex = -1;

static char _trackTitle[64]  = "";
static char _trackArtist[64] = "";

static void _clearMeta() {
  _trackTitle[0]  = '\0';
  _trackArtist[0] = '\0';
}

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
  char path[128];
  snprintf(path, sizeof(path), "%s/%s", MP3_DIR, _names[musicIndex]);
  _audio.stopSong();
  _clearMeta();
  bool ok = _audio.connecttoFS(SD, path);
  _isPlaying = ok;
  if (ok) _playingIndex = musicIndex;
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

void musicLoadCurrent() {
  loadCurrentTrack();
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

void musicSelectNext() {
  if (_trackCount == 0) return;
  musicIndex = (musicIndex + 1) % _trackCount;
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

void musicSelectPrev() {
  if (_trackCount == 0) return;
  musicIndex = (musicIndex - 1 + _trackCount) % _trackCount;
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

int musicPlayingIndex() { return _playingIndex; }

static int _activeIndex() {
  // Calan track varsa onu, yoksa secili index'e dus
  return (_playingIndex >= 0) ? _playingIndex : musicIndex;
}

const char* musicTitle() {
  if (_trackTitle[0] != '\0') return _trackTitle;
  static char fallback[MP3_NAME_MAX];
  const char* nm = musicTrackName(_activeIndex());
  strncpy(fallback, nm, sizeof(fallback) - 1);
  fallback[sizeof(fallback) - 1] = '\0';
  size_t n = strlen(fallback);
  if (n > 4 && (strcmp(fallback + n - 4, ".mp3") == 0 ||
                strcmp(fallback + n - 4, ".MP3") == 0)) {
    fallback[n - 4] = '\0';
  }
  const char* sep = strstr(fallback, " - ");
  if (sep) return sep + 3;
  return fallback;
}
const char* musicArtist() {
  if (_trackArtist[0] != '\0') return _trackArtist;
  static char fallback[MP3_NAME_MAX];
  const char* nm = musicTrackName(_activeIndex());
  const char* sep = strstr(nm, " - ");
  if (!sep) return "";
  size_t len = sep - nm;
  if (len >= sizeof(fallback)) len = sizeof(fallback) - 1;
  memcpy(fallback, nm, len);
  fallback[len] = '\0';
  return fallback;
}
uint32_t musicCurrentSec() { return _audio.getAudioCurrentTime(); }
uint32_t musicTotalSec()   { return _audio.getAudioFileDuration(); }

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

  // Library "Title: ..." / "Artist: ..." formatinda gonderir
  if (strncmp(info, "Title: ", 7) == 0) {
    strncpy(_trackTitle, info + 7, sizeof(_trackTitle) - 1);
    _trackTitle[sizeof(_trackTitle) - 1] = '\0';
  } else if (strncmp(info, "Artist: ", 8) == 0) {
    strncpy(_trackArtist, info + 8, sizeof(_trackArtist) - 1);
    _trackArtist[sizeof(_trackArtist) - 1] = '\0';
  }
}
void audio_bitrate(const char* info) {
  Serial.print("[bitrate] ");
  Serial.println(info);
}
