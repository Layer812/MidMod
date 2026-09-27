// MidMod / Cardputer sample host for the generic MIDI engine
// Original smfPC: Layer8

#include <Arduino.h>
#include <cstring>
#include <cctype>
#include <SdFat.h>
#include <SPI.h>
#include <strings.h>
#include "M5Cardputer.h"
#include "M5UnitSynth.h"

#include <midi_engine.h>
#include <gs2sam_profile_csv.h>
#include <gs2sam_processor.h>

#include "smf/smf_playback.h"
#include "smf/smf_playback_api.h"
#include "smf/smf_probe.h"
#include "ui/cardputer_ui_sample.h"

static constexpr float VERSION = 0.7f;
static constexpr uint8_t SYNTH_VOLUME_DEFAULT = 32;
static constexpr uint8_t SYNTH_VOLUME_STEP = 8;
static constexpr uint8_t BUILTIN_RHYTHM_VOLUME_PERCENT = 88;

/* Sample-host UART pins.  MidiEngine itself is transport/GPIO agnostic.
 * Override these from platformio.ini build_flags for another host/connector,
 * e.g. -DMIDMOD_SYNTH_RX_PIN=13 -DMIDMOD_SYNTH_TX_PIN=14. */
#ifndef MIDMOD_SYNTH_RX_PIN
#define MIDMOD_SYNTH_RX_PIN 1
#endif
#ifndef MIDMOD_SYNTH_TX_PIN
#define MIDMOD_SYNTH_TX_PIN 2
#endif

static constexpr uint8_t SD_SPI_CS_PIN   = 12;
static constexpr uint8_t SD_SPI_MOSI_PIN = 14;
static constexpr uint8_t SD_SPI_SCK_PIN  = 40;
static constexpr uint8_t SD_SPI_MISO_PIN = 39;
static constexpr uint32_t SD_SPI_HZ_FAST = 25000000UL;
static constexpr uint32_t SD_SPI_HZ_SAFE = 10000000UL;

static constexpr int DISPMAX = 7;
static constexpr int LISTMAX = 256;
static constexpr int PATHMAX = 256;
static constexpr int DIRMAX = 10;
static constexpr uint8_t TYPE_UDIR = 0;
static constexpr uint8_t TYPE_SDIR = 1;
static constexpr uint8_t TYPE_SMF  = 2;

struct FileEntry {
  char filename[PATHMAX];
  uint8_t type;
};

M5UnitSynth synth;
SdFat SD;
midi_engine_t g_midi;
gs2sam_processor_t g_gs2sam_processor;
SmfPlayback g_player;
SmfPlaybackApi g_playback_api_adapter;

static FileEntry filelist[LISTMAX];
static int filenum = 0;
static int dirnum = 0;
static char cdir[PATHMAX] = "/";
static char dirs[DIRMAX][PATHMAX] = {"/"};
static int sel = 0;
static int disp = 0;
static bool playAll = false;
static bool nextRequested = false;
static bool stopRequested = false;
static bool loopFlag = false;
static bool helpVisible = false;

// ---- Sample SAM2695 UART transport ------------------------------------------
static void midmodSampleInitSynthUart(int rxPin, int txPin)
{
  synth.begin(&Serial2, UNIT_SYNTH_BAUD, rxPin, txPin);
  Serial.printf("[MIDI OUT] SAM2695 UART RX=%d TX=%d baud=%u\n",
                rxPin, txPin, (unsigned)UNIT_SYNTH_BAUD);
}

// ---- Generic MIDI engine backend ---------------------------------------------
static void midiWrite(void *, const uint8_t *bytes, size_t len)
{
  if (bytes != nullptr && len != 0u) Serial2.write(bytes, len);
}

static void midiSetVolume(void *, uint8_t volume)
{
  synth.setMasterVolume(volume);
  Serial.printf("[VOL] SAM2695 master=%u/127\n", (unsigned)volume);
}

static void changeVolume(int delta)
{
  int v = (int)midi_engine_get_master_volume(&g_midi) + delta;
  if (v < 0) v = 0;
  if (v > 127) v = 127;
  midi_engine_set_master_volume(&g_midi, (uint8_t)v);
}

// ---- Single-CSV GS source profiles -----------------------------------------
static gs2sam_csv_profile_t g_profile_active;
static gs2sam_csv_profile_t g_profile_staging;
static bool g_profile_from_csv = false;
static constexpr size_t PROFILE_MAX = 16;
static constexpr size_t PROFILE_PATH_MAX = 96;
static char g_profile_paths[PROFILE_MAX][PROFILE_PATH_MAX];
static size_t g_profile_count = 0;
static size_t g_profile_index = 0;

static bool hasCsvExtension(const char *name)
{
  if (name == nullptr) return false;
  const char *ext = strrchr(name, '.');
  return ext != nullptr && strlen(ext) == 4u &&
         tolower((unsigned char)ext[1]) == 'c' &&
         tolower((unsigned char)ext[2]) == 's' &&
         tolower((unsigned char)ext[3]) == 'v';
}

static void sortProfilePaths()
{
  for (size_t i = 0; i < g_profile_count; ++i)
    for (size_t j = i + 1; j < g_profile_count; ++j)
      if (strcasecmp(g_profile_paths[i], g_profile_paths[j]) > 0) {
        char tmp[PROFILE_PATH_MAX];
        memcpy(tmp, g_profile_paths[i], sizeof(tmp));
        memcpy(g_profile_paths[i], g_profile_paths[j], PROFILE_PATH_MAX);
        memcpy(g_profile_paths[j], tmp, PROFILE_PATH_MAX);
      }
}

static void scanProfileCsvFiles()
{
  g_profile_count = 0;
  File root = SD.open("/");
  if (!root || !root.isDirectory()) return;
  File file = root.openNextFile();
  while (file && g_profile_count < PROFILE_MAX) {
    if (!file.isDirectory()) {
      char name[PROFILE_PATH_MAX]{};
      file.getName(name, sizeof(name));
      if (hasCsvExtension(name)) {
        snprintf(g_profile_paths[g_profile_count], PROFILE_PATH_MAX,
                 "/%.*s", (int)(PROFILE_PATH_MAX - 2u), name);
        ++g_profile_count;
      }
    }
    file = root.openNextFile();
  }
  sortProfilePaths();
}

static bool readProfileLine(File &file, char *buf, size_t cap, bool &overflow)
{
  size_t n = 0; bool got = false; overflow = false;
  while (file.available()) {
    const int c = file.read();
    if (c < 0) break;
    got = true;
    if (c == '\n') break;
    if (c == '\r') continue;
    if (n + 1u < cap) buf[n++] = (char)c; else overflow = true;
  }
  if (cap != 0u) buf[n] = '\0';
  return got;
}

static const char *activeProfileLabel()
{
  if (!g_profile_from_csv) return "SC-55mkII / GS";
  if (g_profile_active.comment[0] != '\0') return g_profile_active.comment;
  if (g_profile_active.name[0] != '\0') return g_profile_active.name;
  return "GS2SAM CSV";
}

static bool loadCsvProfileFile(const char *path)
{
  File file = SD.open(path);
  if (!file) return false;
  gs2sam_csv_profile_init(&g_profile_staging);
  char line[320]; uint32_t lineNo = 0; bool overflow = false;
  while (readProfileLine(file, line, sizeof(line), overflow)) {
    ++lineNo;
    if (overflow) return false;
    (void)gs2sam_csv_profile_parse_line(&g_profile_staging, line, lineNo);
  }
  if (g_profile_staging.errors != 0u || g_profile_staging.signature_seen == 0u) return false;
  g_profile_active = g_profile_staging;
  g_profile_from_csv = true;
  gs2sam_processor_apply_csv(&g_gs2sam_processor, &g_profile_active);
  midi_engine_t *engine = &g_midi;
  midi_engine_set_profile_label(engine, activeProfileLabel());
  Serial.printf("[PROFILE] loaded %s (%s)\n", path, activeProfileLabel());
  return true;
}

static void activateBuiltinProfile()
{
  g_profile_from_csv = false;
  gs2sam_processor_apply_sc55mk2(&g_gs2sam_processor, BUILTIN_RHYTHM_VOLUME_PERCENT);
  midi_engine_set_profile_label(&g_midi, "SC-55mkII / GS");
  Serial.println("[PROFILE] built-in SC-55mkII fallback");
}

static bool activateProfileIndex(size_t index)
{
  if (index >= g_profile_count || !loadCsvProfileFile(g_profile_paths[index])) return false;
  g_profile_index = index;
  return true;
}

static void activateInitialProfile()
{
  scanProfileCsvFiles();
  for (size_t i = 0; i < g_profile_count; ++i) {
    if (strcasecmp(g_profile_paths[i], "/sc55mk2.csv") == 0) {
      if (activateProfileIndex(i)) return;
      break;
    }
  }
  activateBuiltinProfile();
}

static void cycleProfile()
{
  if (g_profile_count == 0u) return;
  const size_t start = g_profile_from_csv ? g_profile_index : g_profile_count - 1u;
  for (size_t step = 1; step <= g_profile_count; ++step) {
    const size_t next = (start + step) % g_profile_count;
    if (activateProfileIndex(next)) return;
  }
}

// ---- Filer model ------------------------------------------------------------
static bool isMidiFile(const char *fileName)
{
  const char *ext = strrchr(fileName, '.');
  return ext != nullptr && strcasecmp(ext, ".mid") == 0;
}

static int makeFileList()
{
  int i = 0;
  File root = SD.open(cdir);
  if (!root || !root.isDirectory()) return 0;
  memset(filelist, 0, sizeof(filelist));
  if (dirnum > 0) {
    strncpy(filelist[i].filename, "..", PATHMAX - 1);
    filelist[i++].type = TYPE_UDIR;
  }
  File file = root.openNextFile();
  while (file && i < LISTMAX) {
    char fn[PATHMAX]{}; file.getName(fn, sizeof(fn));
    if (file.isDirectory()) {
      strncpy(filelist[i].filename, fn, PATHMAX - 1); filelist[i++].type = TYPE_SDIR;
    } else if (isMidiFile(fn)) {
      strncpy(filelist[i].filename, fn, PATHMAX - 1); filelist[i++].type = TYPE_SMF;
    }
    file = root.openNextFile();
  }
  return i;
}

static void pathForIndex(int index, char out[PATHMAX])
{
  if (index < 0 || index >= filenum) { out[0] = '\0'; return; }
  if (strcmp(cdir, "/") == 0)
    snprintf(out, PATHMAX, "/%.*s", PATHMAX - 2, filelist[index].filename);
  else
    snprintf(out, PATHMAX, "%.*s/%.*s", PATHMAX / 2 - 2, cdir,
             PATHMAX / 2 - 2, filelist[index].filename);
}

static void drawBrowser()
{
  UiFileRow rows[DISPMAX]{};
  const int visible = (filenum - disp) < DISPMAX ? (filenum - disp) : DISPMAX;
  for (int i = 0; i < visible; ++i) {
    rows[i].name = filelist[disp + i].filename;
    rows[i].kind = filelist[disp + i].type;
  }

  SmfProbeInfo info{};
  if (sel >= 0 && sel < filenum && filelist[sel].type == TYPE_SMF) {
    char path[PATHMAX]; pathForIndex(sel, path);
    (void)smfProbeFile(SD, path, info);
  }

  UiBrowserModel model{};
  model.profile = activeProfileLabel();
  model.folder = cdir;
  model.rows = rows;
  model.row_count = visible > 0 ? (size_t)visible : 0u;
  model.selected = sel >= disp ? (size_t)(sel - disp) : 0u;
  model.volume = midi_engine_get_master_volume(&g_midi);
  model.loop = loopFlag;
  model.selected_duration_ms = info.valid ? info.total_ms : 0u;
  model.selected_tracks = info.valid ? (uint8_t)info.tracks : 0u;
  cardputerUiDrawBrowser(model);
}

static void normalizeSelection()
{
  if (filenum <= 0) { sel = disp = 0; return; }
  if (sel < 0) sel = 0;
  if (sel >= filenum) sel = filenum - 1;
  if (sel < disp) disp = sel;
  if (sel >= disp + DISPMAX) disp = sel - DISPMAX + 1;
  if (disp < 0) disp = 0;
}

static bool enterOrBuildPath(int index, char out[PATHMAX])
{
  if (index < 0 || index >= filenum) return false;
  if (filelist[index].type == TYPE_SMF) { pathForIndex(index, out); return true; }
  if (filelist[index].type == TYPE_UDIR) {
    if (dirnum > 0) --dirnum;
    strncpy(cdir, dirs[dirnum], PATHMAX - 1); cdir[PATHMAX - 1] = '\0';
  } else if (filelist[index].type == TYPE_SDIR && dirnum < DIRMAX - 1) {
    ++dirnum;
    if (strcmp(cdir, "/") == 0)
      snprintf(dirs[dirnum], PATHMAX, "/%.*s", PATHMAX - 2, filelist[index].filename);
    else
      snprintf(dirs[dirnum], PATHMAX, "%.*s/%.*s", PATHMAX / 2 - 2, cdir,
               PATHMAX / 2 - 2, filelist[index].filename);
    strncpy(cdir, dirs[dirnum], PATHMAX - 1); cdir[PATHMAX - 1] = '\0';
  }
  sel = disp = 0;
  filenum = makeFileList();
  normalizeSelection();
  drawBrowser();
  return false;
}

static int nextMidiIndex(int from)
{
  for (int i = from + 1; i < filenum; ++i) if (filelist[i].type == TYPE_SMF) return i;
  return -1;
}

static int firstMidiIndex()
{
  for (int i = 0; i < filenum; ++i) if (filelist[i].type == TYPE_SMF) return i;
  return -1;
}

static void printStats()
{
  const gs2sam_stats_t *st = gs2sam_processor_stats(&g_gs2sam_processor);
  if (st == nullptr) return;
  Serial.printf("[GS2SAM] in=%lu out=%lu native=%lu exact=%lu approx=%lu unsupported=%lu resync=%lu\n",
    (unsigned long)st->bytes_in, (unsigned long)st->bytes_out,
    (unsigned long)st->native_pass, (unsigned long)st->exact_translate,
    (unsigned long)st->approximated, (unsigned long)st->unsupported,
    (unsigned long)g_player.sysexResyncs());
  Serial.printf("[MAP] tone_exact=%lu tone_wild=%lu family=%lu capital=%lu tone_rules=%lu "
                "drum_direct=%lu drum_retarget=%lu drum_fallback=%lu drumkit_rules=%lu drumnote_rules=%lu\n",
    (unsigned long)st->tone_exact_lsb_hits, (unsigned long)st->tone_wildcard_lsb_hits,
    (unsigned long)st->tone_family_fallbacks, (unsigned long)st->tone_fallbacks,
    (unsigned long)st->profile_tone_rule_hits, (unsigned long)st->drum_kit_direct,
    (unsigned long)st->drum_kit_retargets, (unsigned long)st->drum_fallbacks,
    (unsigned long)st->profile_drum_kit_rule_hits, (unsigned long)st->profile_drum_note_rule_hits);
  Serial.printf("[MAP] fanout=%lu key_suppress=%lu key_conflict=%lu sysex_unknown_bytes=%lu "
                "sysex_malformed_bytes=%lu emit_cb=%lu emit_max=%lu\n",
    (unsigned long)st->drum_map_fanouts, (unsigned long)st->key_range_filtered,
    (unsigned long)st->key_range_conflicts, (unsigned long)st->unknown_sysex_passthrough_bytes,
    (unsigned long)st->malformed_sysex_passthrough_bytes,
    (unsigned long)st->emit_callbacks, (unsigned long)st->emit_max_callback_bytes);
}

static bool startSelected()
{
  char path[PATHMAX];
  if (!enterOrBuildPath(sel, path)) return false;
  const int err = g_player.load(path);
  if (err != MD_MIDIFile::E_OK) {
    Serial.printf("[SMF] load error %d: %s\n", err, path);
    cardputerUiDrawMessage("SMF ERROR", "load failed");
    delay(700);
    return false;
  }
  g_player.setLoop(loopFlag);
  Serial.printf("[SMF] play %s\n", path);
  return true;
}

static bool menuKeyPressed()
{
  return M5Cardputer.Keyboard.isKeyPressed('m') || M5Cardputer.Keyboard.isKeyPressed('M');
}

static bool enterKeyPressed()
{
  return M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER);
}

static bool backKeyPressed()
{
  return M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE);
}

static void goParentDirectory()
{
  if (dirnum <= 0) return;
  --dirnum;
  strncpy(cdir, dirs[dirnum], PATHMAX - 1);
  cdir[PATHMAX - 1] = '\0';
  sel = disp = 0;
  filenum = makeFileList();
  normalizeSelection();
  drawBrowser();
}

static void handleBrowserKeys(bool &startPlayback)
{
  M5Cardputer.update();
  if (!M5Cardputer.Keyboard.isChange()) return;
  if (menuKeyPressed()) {
    helpVisible = !helpVisible;
    if (helpVisible)
      cardputerUiDrawHelp(false, activeProfileLabel(), midi_engine_get_master_volume(&g_midi), loopFlag);
    else
      drawBrowser();
    return;
  }
  if (backKeyPressed()) {
    if (helpVisible) { helpVisible = false; drawBrowser(); }
    else goParentDirectory();
    return;
  }
  if (helpVisible) return;
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { --sel; normalizeSelection(); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('.')) { ++sel; normalizeSelection(); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed(',')) { sel -= DISPMAX; normalizeSelection(); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('/')) { sel += DISPMAX; normalizeSelection(); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('-')) { changeVolume(-SYNTH_VOLUME_STEP); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('=')) { changeVolume(+SYNTH_VOLUME_STEP); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('l')) { loopFlag = !loopFlag; midi_engine_set_loop(&g_midi, loopFlag); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('p')) { cycleProfile(); drawBrowser(); }
  if (M5Cardputer.Keyboard.isKeyPressed('a')) {
    playAll = true; loopFlag = false; midi_engine_set_loop(&g_midi, 0);
    int i = filelist[sel].type == TYPE_SMF ? sel : firstMidiIndex();
    if (i >= 0) { sel = i; normalizeSelection(); startPlayback = startSelected(); }
  }
  if (M5Cardputer.Keyboard.isKeyPressed(' ') || enterKeyPressed()) {
    playAll = false;
    startPlayback = startSelected();
  }
}

static void handlePlaybackKeys()
{
  M5Cardputer.update();
  if (!M5Cardputer.Keyboard.isChange()) return;
  if (menuKeyPressed()) {
    helpVisible = !helpVisible;
    if (helpVisible)
      cardputerUiDrawHelp(true, activeProfileLabel(), midi_engine_get_master_volume(&g_midi), loopFlag);
    return;
  }
  const mplay_api_t *api = g_playback_api_adapter.api();
  if (backKeyPressed()) {
    if (helpVisible) helpVisible = false;
    else (void)mplay_send_command(api, MPLAY_CMD_STOP, 0);
    return;
  }
  if (helpVisible) return;
  if (M5Cardputer.Keyboard.isKeyPressed(' ') || enterKeyPressed())
    (void)mplay_send_command(api, MPLAY_CMD_PLAY_PAUSE, 0);
  if (M5Cardputer.Keyboard.isKeyPressed(',')) (void)mplay_send_command(api, MPLAY_CMD_RESTART, 0);
  if (M5Cardputer.Keyboard.isKeyPressed('/')) (void)mplay_send_command(api, MPLAY_CMD_NEXT, 0);
  if (M5Cardputer.Keyboard.isKeyPressed('q')) (void)mplay_send_command(api, MPLAY_CMD_STOP, 0);
  if (M5Cardputer.Keyboard.isKeyPressed('-')) (void)mplay_send_command(api, MPLAY_CMD_ADJUST_VOLUME, -SYNTH_VOLUME_STEP);
  if (M5Cardputer.Keyboard.isKeyPressed('=')) (void)mplay_send_command(api, MPLAY_CMD_ADJUST_VOLUME, +SYNTH_VOLUME_STEP);
  if (M5Cardputer.Keyboard.isKeyPressed('l')) {
    loopFlag = !loopFlag;
    (void)mplay_send_command(api, MPLAY_CMD_SET_LOOP, loopFlag ? 1 : 0);
  }
}

void setup()
{
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  Serial.begin(115200);
  Serial.println();
  Serial.println("MidMod 0.1.1 - Modifiable MIDI Module");

  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  pinMode(SD_SPI_CS_PIN, OUTPUT); digitalWrite(SD_SPI_CS_PIN, HIGH);
  bool sdOk = SD.begin(SdSpiConfig(SD_SPI_CS_PIN, SHARED_SPI, SD_SPI_HZ_FAST, &SPI));
  if (!sdOk) sdOk = SD.begin(SdSpiConfig(SD_SPI_CS_PIN, SHARED_SPI, SD_SPI_HZ_SAFE, &SPI));
  if (!sdOk) {
    Serial.println("ERROR: SD init failed"); SD.initErrorPrint(&Serial);
    cardputerUiBegin(); cardputerUiDrawMessage("SD ERROR", "init failed");
    while (true) delay(100);
  }

  midmodSampleInitSynthUart(MIDMOD_SYNTH_RX_PIN, MIDMOD_SYNTH_TX_PIN);
  midi_engine_backend_t backend{midiWrite, midiSetVolume, nullptr};
  midi_engine_init(&g_midi, &backend, SYNTH_VOLUME_DEFAULT);
  gs2sam_processor_init_sc55mk2(&g_gs2sam_processor, &g_midi, BUILTIN_RHYTHM_VOLUME_PERCENT);
  midi_processor_t proc = gs2sam_processor_interface(&g_gs2sam_processor);
  midi_engine_set_processor(&g_midi, &proc);
  midi_engine_set_profile_label(&g_midi, "SC-55mkII / GS");
  activateInitialProfile();
  g_player.begin(&SD, &g_midi);
  g_playback_api_adapter.begin(&g_player, &g_midi, &loopFlag, &nextRequested, &stopRequested);
  cardputerUiBegin();

  strncpy(dirs[0], "/", PATHMAX - 1);
  filenum = makeFileList();
  normalizeSelection();
  drawBrowser();
  Serial.printf("READY profile=%s volume=%u\n", activeProfileLabel(), (unsigned)midi_engine_get_master_volume(&g_midi));
}

void loop()
{
  static bool playing = false;
  static uint32_t lastMs = (uint32_t)millis();
  static uint32_t lastUiMs = 0;

  const uint32_t now = (uint32_t)millis();
  midi_engine_tick(&g_midi, now - lastMs);
  lastMs = now;

  if (!playing) {
    bool start = false;
    handleBrowserKeys(start);
    if (start) {
      playing = true; nextRequested = stopRequested = false;
      helpVisible = false;
      lastUiMs = 0;
    }
    delay(1);
    return;
  }

  handlePlaybackKeys();
  SmfPlayback::PollResult r = g_player.poll();
  if (!helpVisible && now - lastUiMs >= 50u) {
    mplay_snapshot_t snap{};
    if (mplay_get_snapshot(g_playback_api_adapter.api(), &snap) == MPLAY_OK)
      cardputerUiDrawPlayback(snap);
    lastUiMs = now;
  }

  const bool ended = (r == SmfPlayback::ENDED);
  if (stopRequested || nextRequested || ended) {
    g_player.stop();
    printStats();
    const bool advance = nextRequested || playAll;
    stopRequested = nextRequested = false;
    if (advance) {
      int ni = nextMidiIndex(sel);
      if (ni >= 0) {
        sel = ni; normalizeSelection();
        if (startSelected()) { lastUiMs = 0; delay(1); return; }
      }
    }
    playAll = false;
    playing = false;
    helpVisible = false;
    drawBrowser();
  }
  delay(1);
}
