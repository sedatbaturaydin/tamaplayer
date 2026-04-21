#include "MusicPlayer.h"
#include "../../Config.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

static DFRobotDFPlayerMini _dfp;
static int  _trackCount = 0;
static bool _started    = false;
static bool _dfpOk      = false;

static int      _actualState   = -1;
static int      _actualFileNum = -1;
static uint32_t _queryAt       = 0;  // 0 = query yok; aksi halde ne zaman sorgu yapilacak

bool musicPlaying = false;
int  musicIndex   = 0;

static MusicRedraw _redraw = MUSIC_REDRAW_FULL;

// Non-blocking init state machine (setup'i bloklamamak icin)
enum InitStep : uint8_t {
  INIT_WAIT_BOOT,     // DFPlayer kendi boot'unu bitirsin
  INIT_BEGIN,         // _dfp.begin cagir
  INIT_WAIT_SD,       // begin sonrasi SD mount bekle
  INIT_READ_COUNT,    // readFileCounts retry
  INIT_READY,         // hazir, normal event handling
  INIT_FAILED         // basarisiz
};
static InitStep _initStep       = INIT_WAIT_BOOT;
static uint32_t _initTimer      = 0;
static uint8_t  _beginAttempts  = 0;
static uint8_t  _countAttempts  = 0;

static void setRedraw(MusicRedraw level) {
  if (level > _redraw) _redraw = level;
}

void musicInit() {
  // setup()'u BLOKLAMAZ - sadece UART'i ac ve state machine'i baslat.
  // RX'te ic pullup: floating/gurultuyu biraz bastirir.
  pinMode(DFP_RX, INPUT_PULLUP);
  Serial1.begin(9600, SERIAL_8N1, DFP_RX, DFP_TX);
  _dfp.setTimeOut(1000);

  _initStep      = INIT_WAIT_BOOT;
  _initTimer     = millis();
  _beginAttempts = 0;
  _countAttempts = 0;
  _dfpOk         = false;

  musicPlaying = false;
  musicIndex   = 0;
  _started     = false;
  _redraw      = MUSIC_REDRAW_FULL;
}

// Init state machine'i bir adim ilerletir. Bloklama: begin ~1s, readFileCounts ~0.3s (timeout 300ms).
static void stepInit() {
  uint32_t now = millis();
  switch (_initStep) {
    case INIT_WAIT_BOOT:
      // DFPlayer guc sonrasi ~2sn kendi boot'unu yapiyor, bu sure butonlar yasiyor.
      if (now - _initTimer >= 2000) {
        _initStep  = INIT_BEGIN;
        _initTimer = now;
      }
      break;

    case INIT_BEGIN: {
      // Her cagrida sadece 1 begin denemesi yap (~1s bloke).
      _beginAttempts++;
      bool ok = _dfp.begin(Serial1, /*isACK=*/false, /*doReset=*/true);
      if (ok) {
        _dfpOk     = true;
        _initStep  = INIT_WAIT_SD;
        _initTimer = millis();
      } else if (_beginAttempts >= 3) {
        Serial.println("[DFP] begin failed permanently");
        _initStep = INIT_FAILED;
      }
      break;
    }

    case INIT_WAIT_SD:
      // PDF'e gore init 3-5sn. 3sn bekle.
      if (now - _initTimer >= 3000) {
        _dfp.volume(DFP_VOLUME);
        _dfp.outputDevice(DFPLAYER_DEVICE_SD);  // SD'yi force et
        _dfp.enableDAC();                       // 3.5mm/DAC cikisi ac
        _dfp.setTimeOut(400);
        _initStep      = INIT_READ_COUNT;
        _initTimer     = now;
        _countAttempts = 0;
      }
      break;

    case INIT_READ_COUNT: {
      if (now - _initTimer < 400) break;
      _initTimer = now;
      int count = _dfp.readFileCounts();
      _countAttempts++;
      if (count > 0) {
        _trackCount = count;
        _dfp.setTimeOut(1000);
        Serial.print("[DFP] tracks = "); Serial.println(_trackCount);
        _initStep = INIT_READY;
        setRedraw(MUSIC_REDRAW_FULL);
      } else if (_countAttempts >= 10) {
        // Count guvenilmez cikti; yine de playMp3Folder denenebilsin
        // diye 2 parca varsay (kullanici 2 dosya koydugunu biliyor).
        Serial.println("[DFP] count unreliable, assuming 2 tracks");
        _trackCount = 2;
        _dfp.setTimeOut(1000);
        _initStep = INIT_READY;
        setRedraw(MUSIC_REDRAW_FULL);
      }
      break;
    }

    default:
      break;
  }
}

static void logDfpEvent(uint8_t type, int value) {
  switch (type) {
    case DFPlayerCardInserted: Serial.println("[DFP] evt: Card Inserted"); break;
    case DFPlayerCardRemoved:  Serial.println("[DFP] evt: Card Removed");  break;
    case DFPlayerCardOnline:   Serial.println("[DFP] evt: Card Online");   break;
    case DFPlayerUSBInserted:  Serial.println("[DFP] evt: USB Inserted");  break;
    case DFPlayerPlayFinished:
      Serial.print("[DFP] evt: Finished #"); Serial.println(value);
      break;
    case DFPlayerError:
      Serial.print("[DFP] evt: Error ");
      switch (value) {
        case Busy:             Serial.println("Card not found"); break;
        case Sleeping:         Serial.println("Sleeping");        break;
        case SerialWrongStack: Serial.println("WrongStack");      break;
        case CheckSumNotMatch: Serial.println("ChkSum");          break;
        case FileIndexOut:     Serial.println("IndexOut");        break;
        case FileMismatch:     Serial.println("FileMismatch");    break;
        case Advertise:        Serial.println("Advertise");       break;
        default:               Serial.println(value);             break;
      }
      break;
    case TimeOut: /* too noisy */ break;
    default:
      Serial.print("[DFP] evt: type=0x"); Serial.print(type, HEX);
      Serial.print(" val="); Serial.println(value);
      break;
  }
}

void musicUpdate() {
  if (_dfpOk && _dfp.available()) {
    uint8_t type  = _dfp.readType();
    int     value = _dfp.read();
    logDfpEvent(type, value);
    if (_initStep == INIT_READY && type == DFPlayerPlayFinished) {
      musicNext();
    }
  }

  // Schedule'landiysa state sorgusu yap
  if (_initStep == INIT_READY && _queryAt != 0 && millis() >= _queryAt) {
    _actualState   = _dfp.readState();
    _actualFileNum = _dfp.readCurrentFileNumber();
    int vol        = _dfp.readVolume();
    Serial.print("[DFP] state="); Serial.print(_actualState);
    Serial.print(" curFile="); Serial.print(_actualFileNum);
    Serial.print(" vol="); Serial.println(vol);
    _queryAt = 0;
    setRedraw(MUSIC_REDRAW_PARTIAL);
  }

  if (_initStep != INIT_READY && _initStep != INIT_FAILED) {
    stepInit();
  }
}

int musicActualState()   { return _actualState; }
int musicActualFileNum() { return _actualFileNum; }

static void schedulePlaybackQuery() {
  _queryAt = millis() + 600;
}

void musicTogglePlay() {
  if (!_dfpOk || _trackCount == 0) return;

  if (!_started) {
    _dfp.playMp3Folder(musicIndex + 1);
    _started     = true;
    musicPlaying = true;
    schedulePlaybackQuery();
  } else {
    musicPlaying = !musicPlaying;
    if (musicPlaying) _dfp.start();
    else              _dfp.pause();
    schedulePlaybackQuery();
  }
  setRedraw(MUSIC_REDRAW_PARTIAL);
}

void musicNext() {
  if (!_dfpOk || _trackCount == 0) return;
  musicIndex = (musicIndex + 1) % _trackCount;
  _dfp.playMp3Folder(musicIndex + 1);
  _started     = true;
  musicPlaying = true;
  schedulePlaybackQuery();
  setRedraw(MUSIC_REDRAW_FULL);
}

void musicPrev() {
  if (!_dfpOk || _trackCount == 0) return;
  musicIndex = (musicIndex - 1 + _trackCount) % _trackCount;
  _dfp.playMp3Folder(musicIndex + 1);
  _started     = true;
  musicPlaying = true;
  schedulePlaybackQuery();
  setRedraw(MUSIC_REDRAW_FULL);
}

int musicTrackCount() { return _trackCount; }

MusicRedraw musicRedrawNeeded()   { return _redraw; }
void        musicClearRedrawFlag() { _redraw = MUSIC_REDRAW_NONE; }
