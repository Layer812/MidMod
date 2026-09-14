#include "cardputer_ui_sample.h"

#include "M5Cardputer.h"
#include <stdio.h>
#include <string.h>

namespace {
M5Canvas g_canvas(&M5Cardputer.Display);
bool g_ready = false;
static constexpr uint16_t C_BG = 0x0000;
static constexpr uint16_t C_AMBER = 0xFD20;
static constexpr uint16_t C_AMBER_DIM = 0x7A00;
static constexpr uint16_t C_AMBER_DARK = 0x3100;

static bool hasUtf8(const char *s)
{
  if (s == nullptr) return false;
  while (*s != '\0') {
    if ((unsigned char)*s >= 0x80u) return true;
    ++s;
  }
  return false;
}

static size_t utf8Unit(const char *s)
{
  const unsigned char c = (unsigned char)*s;
  if (c < 0x80u) return 1u;
  if ((c & 0xE0u) == 0xC0u) return 2u;
  if ((c & 0xF0u) == 0xE0u) return 3u;
  if ((c & 0xF8u) == 0xF0u) return 4u;
  return 1u;
}

static void clipUtf8Chars(char *dst, size_t cap, const char *src, size_t maxChars)
{
  if (cap == 0u) return;
  if (src == nullptr) src = "";
  size_t out = 0u;
  size_t chars = 0u;
  while (*src != '\0' && chars < maxChars) {
    const size_t unit = utf8Unit(src);
    size_t avail = strlen(src);
    size_t copy = unit <= avail ? unit : 1u;
    if (out + copy + 1u > cap) break;
    memcpy(dst + out, src, copy);
    out += copy;
    src += copy;
    ++chars;
  }
  dst[out] = '\0';
}

static void clipText(char *dst, size_t cap, const char *src, size_t maxChars)
{
  clipUtf8Chars(dst, cap, src, maxChars);
}

static void useAscii(uint8_t size = 1)
{
  g_canvas.setTextFont(1);
  g_canvas.setTextSize(size);
}

static void useJapanese(uint8_t px, bool bold = false)
{
  g_canvas.setTextSize(1);
  if (px >= 16u) g_canvas.setFont(bold ? &fonts::efontJA_16_b : &fonts::efontJA_16);
  else if (px >= 14u) g_canvas.setFont(bold ? &fonts::efontJA_14_b : &fonts::efontJA_14);
  else if (px >= 12u) g_canvas.setFont(bold ? &fonts::efontJA_12_b : &fonts::efontJA_12);
  else g_canvas.setFont(bold ? &fonts::efontJA_10_b : &fonts::efontJA_10);
}

static void useFontFor(const char *text, uint8_t asciiSize, uint8_t jaPx, bool bold = false)
{
  if (hasUtf8(text)) useJapanese(jaPx, bold);
  else useAscii(asciiSize);
}

static void fitUtf8Px(char *dst, size_t cap, const char *src, int maxWidth)
{
  if (cap == 0u) return;
  if (src == nullptr) src = "";
  size_t out = 0u;
  dst[0] = '\0';
  while (*src != '\0') {
    const size_t unit = utf8Unit(src);
    const size_t avail = strlen(src);
    const size_t copy = unit <= avail ? unit : 1u;
    if (out + copy + 1u > cap) break;
    memcpy(dst + out, src, copy);
    out += copy;
    dst[out] = '\0';
    if (g_canvas.textWidth(dst) > maxWidth) {
      out -= copy;
      dst[out] = '\0';
      break;
    }
    src += copy;
  }
}

static void fmtTime(char out[12], uint32_t ms)
{
  const uint32_t sec = ms / 1000u;
  const uint32_t min = sec / 60u;
  snprintf(out, 12, "%02lu:%02lu", (unsigned long)min, (unsigned long)(sec % 60u));
}

static const char *stateText(mplay_state_t s)
{
  switch (s) {
    case MPLAY_PLAYING: return "PLAY";
    case MPLAY_PAUSED: return "PAUSE";
    default: return "STOP";
  }
}

static void frameStart()
{
  if (!g_ready) cardputerUiBegin();
  g_canvas.fillSprite(C_BG);
  useAscii(1);
  g_canvas.setTextWrap(false);
}

/* MidMod: the product mark is shown prominently on every screen.
   File/profile strings may be UTF-8 Japanese; those paths use M5GFX efontJA. */
static void topBar(const char *text)
{
  g_canvas.fillRect(0, 0, 240, 23, C_AMBER);

  useAscii(2);
  const int markW = g_canvas.textWidth("MidMod");
  const int markX = 238 - markW;
  g_canvas.setTextColor(C_BG, C_AMBER);
  g_canvas.setCursor(markX, 3);
  g_canvas.print("MidMod");

  useFontFor(text, 2, 16, true);
  g_canvas.setTextColor(C_BG, C_AMBER);
  char tmp[96];
  fitUtf8Px(tmp, sizeof(tmp), text, markX - 7);
  g_canvas.setCursor(3, hasUtf8(text) ? 3 : 3);
  g_canvas.print(tmp);
}

static void profileRow(const char *profile, const char *mode)
{
  useFontFor(profile, 2, 14, true);
  g_canvas.setTextColor(C_AMBER, C_BG);
  char p[96];
  fitUtf8Px(p, sizeof(p), profile, 187);
  g_canvas.setCursor(4, hasUtf8(profile) ? 26 : 25);
  g_canvas.print(p);

  if (mode != nullptr) {
    useAscii(1);
    char m[9]; clipText(m, sizeof(m), mode, 7);
    g_canvas.setCursor(196, 30);
    g_canvas.print(m);
  }
}

static void push() { g_canvas.pushSprite(0, 0); }
}

void cardputerUiBegin()
{
  if (g_ready) return;
  g_canvas.setColorDepth(16);
  if (g_canvas.createSprite(240, 135) == nullptr) return;
  g_ready = true;
}

void cardputerUiDrawPlayback(const mplay_snapshot_t &s)
{
  frameStart();
  topBar(s.title[0] ? s.title : "MIDI");
  profileRow(s.profile, stateText(s.state));

  char a[12], b[12];
  fmtTime(a, s.elapsed_ms); fmtTime(b, s.total_ms);
  useAscii(1);
  g_canvas.setTextColor(C_AMBER, C_BG);
  g_canvas.setCursor(4, 44);
  g_canvas.printf("%s / %s", a, s.total_ms ? b : "--:--");
  g_canvas.setCursor(168, 44);
  g_canvas.printf("VOL %u", (unsigned)s.master_volume);
  g_canvas.setCursor(168, 54);
  g_canvas.printf("LOOP %s", s.loop_enabled ? "ON" : "OFF");

  const int px = 4, py = 57, pw = 158, ph = 6;
  g_canvas.drawRect(px, py, pw, ph, C_AMBER_DIM);
  uint32_t fill = 0;
  if (s.total_ms != 0u) fill = (uint32_t)(((uint64_t)(pw - 2) * s.elapsed_ms) / s.total_ms);
  if (fill > (uint32_t)(pw - 2)) fill = pw - 2;
  if (fill != 0u) g_canvas.fillRect(px + 1, py + 1, (int)fill, ph - 2, C_AMBER);

  /* Compact MIDI-note-derived spectrum.
     The playback snapshot keeps 24 bins for API users, but the small
     Cardputer sample deliberately renders 12 wider-spaced bars.  This
     keeps the right-hand frame visible and gives the graph a cleaner
     hardware-analyser look at 240x135. */
  const int sx = 42, sy = 76, sw = 146, sh = 50;
  useAscii(1);
  g_canvas.setTextColor(C_AMBER_DIM, C_BG);
  g_canvas.setCursor(1, 76);  g_canvas.print("0");
  g_canvas.setCursor(1, 96);  g_canvas.print("-24");
  g_canvas.setCursor(1, 116); g_canvas.print("-48");

  /* Draw the complete analyser frame first; bars stay inside it. */
  g_canvas.drawRect(sx, sy, sw, sh, C_AMBER_DIM);
  for (int gy = sy + 10; gy < sy + sh - 1; gy += 10)
    g_canvas.drawFastHLine(sx + 1, gy, sw - 2, C_AMBER_DARK);

  static constexpr size_t kDisplayBars = 12u;
  const int bw = 8;
  const int gap = 3;
  const int barsW = (int)kDisplayBars * bw + ((int)kDisplayBars - 1) * gap;
  const int barsX = sx + (sw - barsW) / 2;
  const int baseY = sy + sh - 2;
  const int maxH = sh - 4;

  for (size_t bar = 0; bar < kDisplayBars; ++bar) {
    /* Downsample 24 API bins to 12 display bars using peak-hold grouping.
       Keeping this in the UI means the MidiPlayback API resolution does
       not change for PX68 or other front ends. */
    const size_t first = (bar * MPLAY_SPECTRUM_BINS) / kDisplayBars;
    size_t last = ((bar + 1u) * MPLAY_SPECTRUM_BINS) / kDisplayBars;
    if (last <= first) last = first + 1u;
    if (last > MPLAY_SPECTRUM_BINS) last = MPLAY_SPECTRUM_BINS;

    uint8_t level = 0u;
    for (size_t i = first; i < last; ++i)
      if (s.spectrum[i] > level) level = s.spectrum[i];

    int h = ((int)level * maxH) / 127;
    if (h < 0) h = 0;
    if (h > maxH) h = maxH;

    const int x = barsX + (int)bar * (bw + gap);
    g_canvas.drawRect(x, sy + 2, bw, sh - 4, C_AMBER_DARK);
    if (h > 0) {
      for (int yy = baseY; yy > baseY - h; yy -= 5) {
        const int top = yy - 3;
        if (top <= sy + 1) break;
        g_canvas.fillRect(x + 1, top, bw - 2, 3, C_AMBER);
      }
    }
  }
  push();
}

void cardputerUiDrawBrowser(const UiBrowserModel &m)
{
  frameStart();
  const char *selectedName = (m.rows != nullptr && m.selected < m.row_count)
                           ? m.rows[m.selected].name : (m.folder ? m.folder : "/");
  topBar(selectedName);
  profileRow(m.profile, "BROWSE");
  g_canvas.drawFastHLine(0, 43, 240, C_AMBER_DIM);

  static constexpr int listW = 169;
  static constexpr int rowH = 12;
  static constexpr int y0 = 45;
  for (size_t i = 0; i < 7u; ++i) {
    const int y = y0 + (int)i * rowH;
    if (i >= m.row_count) break;
    const bool selected = (i == m.selected);
    if (selected) {
      g_canvas.fillRect(0, y, listW - 3, rowH - 1, C_AMBER);
      g_canvas.setTextColor(C_BG, C_AMBER);
    } else {
      g_canvas.setTextColor(C_AMBER, C_BG);
    }
    const char marker = m.rows[i].kind == 2u ? '>' : (m.rows[i].kind == 1u ? 'D' : '^');
    char prefix[8];
    snprintf(prefix, sizeof(prefix), "%c%02u ", marker, (unsigned)(i + 1u));
    useFontFor(m.rows[i].name, 1, 10, false);
    g_canvas.setTextColor(selected ? C_BG : C_AMBER, selected ? C_AMBER : C_BG);
    g_canvas.setCursor(3, y + 1);
    g_canvas.print(prefix);
    const int nameX = g_canvas.getCursorX();
    char name[128];
    fitUtf8Px(name, sizeof(name), m.rows[i].name, listW - nameX - 5);
    g_canvas.print(name);
    g_canvas.drawFastHLine(0, y + rowH - 1, listW - 3, C_AMBER_DARK);
  }

  g_canvas.drawRect(listW - 3, y0, 3, 84, C_AMBER_DIM);
  if (m.row_count > 0u) {
    int thumbY = y0 + 1 + (int)((m.selected * 74u) / m.row_count);
    g_canvas.fillRect(listW - 2, thumbY, 1, 9, C_AMBER);
  }

  const int ix = 173;
  g_canvas.drawRect(ix, y0, 67, 88, C_AMBER_DIM);
  g_canvas.setTextColor(C_AMBER, C_BG);
  useFontFor(selectedName, 1, 10, false);
  char shortName[96];
  fitUtf8Px(shortName, sizeof(shortName), selectedName, 58);
  g_canvas.setCursor(ix + 4, y0 + 3);
  g_canvas.print(shortName);
  g_canvas.drawFastHLine(ix + 4, y0 + 16, 59, C_AMBER_DIM);

  useAscii(1);
  char t[12]; fmtTime(t, m.selected_duration_ms);
  g_canvas.setCursor(ix + 4, y0 + 22); g_canvas.printf("TIME %s", m.selected_duration_ms ? t : "--:--");
  g_canvas.setCursor(ix + 4, y0 + 34); g_canvas.printf("TRK  %u", (unsigned)m.selected_tracks);
  g_canvas.setCursor(ix + 4, y0 + 46); g_canvas.printf("VOL  %u", (unsigned)m.volume);
  g_canvas.setCursor(ix + 4, y0 + 58); g_canvas.printf("LOOP %s", m.loop ? "ON" : "OFF");

  const char *folderSrc = m.folder != nullptr ? m.folder : "/";
  useFontFor(folderSrc, 1, 10, false);
  char folder[96];
  fitUtf8Px(folder, sizeof(folder), folderSrc, 58);
  g_canvas.setCursor(ix + 4, y0 + 72);
  g_canvas.print(folder);
  push();
}

void cardputerUiDrawHelp(bool playback, const char *profile, uint8_t volume, bool loop)
{
  frameStart();
  topBar("操作ガイド");

  useFontFor(profile, 1, 10, false);
  g_canvas.setTextColor(C_AMBER, C_BG);
  char p[96]; fitUtf8Px(p, sizeof(p), profile, 136);
  g_canvas.setCursor(4, 26);
  g_canvas.print(p);
  useAscii(1);
  g_canvas.setCursor(150, 27);
  g_canvas.printf("V%u L:%s", (unsigned)volume, loop ? "ON" : "OFF");
  g_canvas.drawFastHLine(0, 37, 240, C_AMBER_DIM);

  static const char *browserLines[] = {
    "ブラウザ",
    "; / .       上下移動",
    ", / /       ページ移動",
    "ENTER/SPACE 開く・再生",
    "BACKSPACE   上の画面へ",
    "A           フォルダ全再生",
    "P           音源プロファイル",
    "- / =       音量 下 / 上",
    "L           ループ ON/OFF",
    "M           メニューを閉じる"
  };
  static const char *playLines[] = {
    "再生中",
    "ENTER/SPACE 一時停止/再開",
    ",           曲頭へ戻る",
    "/           次の曲",
    "BACKSPACE   ブラウザへ戻る",
    "Q           停止/ブラウザ",
    "- / =       音量 下 / 上",
    "L           ループ ON/OFF",
    "M           メニューを閉じる"
  };
  const char *const *lines = playback ? playLines : browserLines;
  const size_t count = playback ? sizeof(playLines) / sizeof(playLines[0])
                                : sizeof(browserLines) / sizeof(browserLines[0]);
  useJapanese(10, false);
  for (size_t i = 0; i < count; ++i) {
    g_canvas.setCursor(5, 40 + (int)i * 9);
    g_canvas.setTextColor(i == 0u ? C_AMBER : C_AMBER_DIM, C_BG);
    g_canvas.print(lines[i]);
  }
  push();
}

void cardputerUiDrawMessage(const char *title, const char *message)
{
  frameStart();
  topBar(title != nullptr ? title : "MidMod");
  const char *msg = message != nullptr ? message : "";
  useFontFor(msg, 2, 16, true);
  g_canvas.setTextColor(C_AMBER, C_BG);
  g_canvas.setCursor(6, 34);
  g_canvas.println(msg);
  push();
}
