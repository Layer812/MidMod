#include "smf_probe.h"

#include <string.h>

namespace {
struct TempoPoint { uint32_t tick; uint32_t us_qn; };
static constexpr size_t TEMPO_MAX = 128;

static bool readByte(File &f, uint8_t &v)
{
  int c = f.read();
  if (c < 0) return false;
  v = (uint8_t)c;
  return true;
}

static bool readBE16(File &f, uint16_t &v)
{
  uint8_t a, b;
  if (!readByte(f, a) || !readByte(f, b)) return false;
  v = (uint16_t)(((uint16_t)a << 8) | b);
  return true;
}

static bool readBE32(File &f, uint32_t &v)
{
  uint8_t a,b,c,d;
  if (!readByte(f,a) || !readByte(f,b) || !readByte(f,c) || !readByte(f,d)) return false;
  v = ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | d;
  return true;
}

static bool readVlv(File &f, uint32_t &remaining, uint32_t &v)
{
  v = 0;
  for (uint8_t i = 0; i < 4; ++i) {
    if (remaining == 0) return false;
    uint8_t b;
    if (!readByte(f, b)) return false;
    --remaining;
    v = (v << 7) | (uint32_t)(b & 0x7f);
    if ((b & 0x80u) == 0) return true;
  }
  return false;
}

static bool skipN(File &f, uint32_t &remaining, uint32_t n)
{
  if (n > remaining) return false;
  while (n-- != 0u) {
    uint8_t b;
    if (!readByte(f, b)) return false;
    --remaining;
  }
  return true;
}

static uint8_t channelDataLen(uint8_t status)
{
  switch (status & 0xf0u) {
    case 0xc0u: case 0xd0u: return 1;
    case 0x80u: case 0x90u: case 0xa0u: case 0xb0u: case 0xe0u: return 2;
    default: return 0;
  }
}

static void sortTempo(TempoPoint *p, size_t n)
{
  for (size_t i = 1; i < n; ++i) {
    TempoPoint x = p[i];
    size_t j = i;
    while (j > 0 && p[j - 1].tick > x.tick) { p[j] = p[j - 1]; --j; }
    p[j] = x;
  }
}
}

bool smfProbeFile(SdFat &sd, const char *path, SmfProbeInfo &out)
{
  memset(&out, 0, sizeof(out));
  out.initial_bpm = 120;
  if (path == nullptr) return false;

  File f = sd.open(path);
  if (!f) return false;

  char id[5] = {0};
  if (f.read(id, 4) != 4 || memcmp(id, "MThd", 4) != 0) return false;
  uint32_t headerLen;
  if (!readBE32(f, headerLen) || headerLen < 6u) return false;
  uint16_t fmt, tracks, division;
  if (!readBE16(f, fmt) || !readBE16(f, tracks) || !readBE16(f, division)) return false;
  for (uint32_t i = 6; i < headerLen; ++i) { uint8_t b; if (!readByte(f,b)) return false; }

  out.format = fmt;
  out.tracks = tracks;
  if ((division & 0x8000u) != 0u || division == 0u) return false; // SMPTE timing not handled
  out.tpqn = division;

  TempoPoint tempos[TEMPO_MAX];
  size_t tempoCount = 0;
  uint32_t maxTick = 0;

  for (uint16_t tr = 0; tr < tracks; ++tr) {
    if (f.read(id, 4) != 4) return false;
    uint32_t trackLen;
    if (!readBE32(f, trackLen)) return false;
    if (memcmp(id, "MTrk", 4) != 0) return false;

    uint32_t rem = trackLen;
    uint32_t tick = 0;
    uint8_t running = 0;
    while (rem > 0u) {
      uint32_t delta;
      if (!readVlv(f, rem, delta)) return false;
      tick += delta;
      if (tick > maxTick) maxTick = tick;
      if (rem == 0u) break;

      uint8_t b;
      if (!readByte(f, b)) return false;
      --rem;
      bool firstData = false;
      uint8_t status = b;
      uint8_t data0 = 0;
      if (b < 0x80u) {
        if (running == 0u) return false;
        status = running;
        firstData = true;
        data0 = b;
        (void)data0;
      }

      if (status == 0xffu) {
        running = 0;
        if (firstData || rem == 0u) return false;
        uint8_t type;
        if (!readByte(f, type)) return false;
        --rem;
        uint32_t len;
        if (!readVlv(f, rem, len)) return false;
        if (type == 0x51u && len == 3u) {
          if (rem < 3u) return false;
          uint8_t t0,t1,t2;
          if (!readByte(f,t0) || !readByte(f,t1) || !readByte(f,t2)) return false;
          rem -= 3u;
          uint32_t usqn = ((uint32_t)t0 << 16) | ((uint32_t)t1 << 8) | t2;
          if (usqn != 0u && tempoCount < TEMPO_MAX) tempos[tempoCount++] = {tick, usqn};
        } else {
          if (!skipN(f, rem, len)) return false;
        }
      } else if (status == 0xf0u || status == 0xf7u) {
        running = 0;
        if (firstData) return false;
        uint32_t len;
        if (!readVlv(f, rem, len) || !skipN(f, rem, len)) return false;
      } else if (status >= 0x80u && status <= 0xefu) {
        const uint8_t need = channelDataLen(status);
        if (need == 0u) return false;
        running = status;
        uint8_t have = firstData ? 1u : 0u;
        while (have < need) {
          if (rem == 0u) return false;
          uint8_t d; if (!readByte(f,d)) return false;
          --rem; ++have;
        }
      } else {
        /* System messages are not valid directly in SMF; fail rather than
           invent lengths and return a misleading duration. */
        return false;
      }
    }
  }

  sortTempo(tempos, tempoCount);
  uint32_t tempo = 500000u;
  uint32_t prevTick = 0u;
  uint64_t totalUs = 0u;
  bool initialSet = false;
  for (size_t i = 0; i < tempoCount; ++i) {
    const uint32_t t = tempos[i].tick > maxTick ? maxTick : tempos[i].tick;
    if (t > prevTick) {
      totalUs += ((uint64_t)(t - prevTick) * tempo) / division;
      prevTick = t;
    }
    tempo = tempos[i].us_qn;
    if (!initialSet && tempos[i].tick == 0u && tempo != 0u) {
      out.initial_bpm = (uint16_t)(60000000u / tempo);
      initialSet = true;
    }
    if (tempos[i].tick > maxTick) break;
  }
  if (maxTick > prevTick)
    totalUs += ((uint64_t)(maxTick - prevTick) * tempo) / division;

  out.total_ms = (uint32_t)(totalUs / 1000u);
  out.valid = true;
  return true;
}
