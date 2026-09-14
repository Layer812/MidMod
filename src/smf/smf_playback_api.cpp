#include "smf_playback_api.h"

#include <string.h>

namespace {
static void copyText(char *dst, size_t cap, const char *src)
{
  if (dst == nullptr || cap == 0u) return;
  if (src == nullptr) src = "";
  size_t n = strlen(src);
  if (n >= cap) n = cap - 1u;
  memcpy(dst, src, n);
  dst[n] = '\0';
}

static mplay_state_t convertState(midi_engine_play_state_t s)
{
  if (s == MIDI_ENGINE_PLAYING) return MPLAY_PLAYING;
  if (s == MIDI_ENGINE_PAUSED) return MPLAY_PAUSED;
  return MPLAY_STOPPED;
}
}

void SmfPlaybackApi::begin(SmfPlayback *player,
                           midi_engine_t *engine,
                           bool *loop_flag,
                           bool *next_requested,
                           bool *stop_requested)
{
  _player = player;
  _engine = engine;
  _loop_flag = loop_flag;
  _next_requested = next_requested;
  _stop_requested = stop_requested;
  _api.capabilities = MPLAY_CAP_PLAY_PAUSE |
                      MPLAY_CAP_STOP |
                      MPLAY_CAP_RESTART |
                      MPLAY_CAP_NEXT |
                      MPLAY_CAP_LOOP |
                      MPLAY_CAP_VOLUME |
                      MPLAY_CAP_CHANNEL_METER |
                      MPLAY_CAP_SPECTRUM;
  _api.get_snapshot = snapshotThunk;
  _api.command = commandThunk;
  _api.user = this;
}

mplay_result_t SmfPlaybackApi::snapshotThunk(void *user, mplay_snapshot_t *out)
{
  if (user == nullptr) return MPLAY_INVALID;
  return static_cast<SmfPlaybackApi *>(user)->getSnapshot(out);
}

mplay_result_t SmfPlaybackApi::commandThunk(void *user, mplay_command_t command, int32_t value)
{
  if (user == nullptr) return MPLAY_INVALID;
  return static_cast<SmfPlaybackApi *>(user)->command(command, value);
}

mplay_result_t SmfPlaybackApi::getSnapshot(mplay_snapshot_t *out) const
{
  if (_engine == nullptr || out == nullptr) return MPLAY_INVALID;
  midi_engine_snapshot_t src{};
  midi_engine_get_snapshot(_engine, &src);
  memset(out, 0, sizeof(*out));
  copyText(out->title, sizeof(out->title), src.title);
  copyText(out->profile, sizeof(out->profile), src.profile);
  out->state = convertState(src.play_state);
  out->master_volume = src.master_volume;
  out->loop_enabled = src.loop_enabled;
  out->tempo_bpm = src.tempo_bpm;
  out->elapsed_ms = src.elapsed_ms;
  out->total_ms = src.total_ms;
  for (size_t i = 0; i < MPLAY_CHANNELS && i < MIDI_ENGINE_CHANNELS; ++i)
    out->channel_activity[i] = src.channel_activity[i];
  for (size_t i = 0; i < MPLAY_SPECTRUM_BINS && i < MIDI_ENGINE_SPECTRUM_BINS; ++i)
    out->spectrum[i] = src.spectrum[i];
  return MPLAY_OK;
}

mplay_result_t SmfPlaybackApi::command(mplay_command_t command, int32_t value)
{
  if (_player == nullptr || _engine == nullptr) return MPLAY_INVALID;
  switch (command) {
    case MPLAY_CMD_PLAY_PAUSE:
      _player->togglePaused();
      return MPLAY_OK;
    case MPLAY_CMD_STOP:
      if (_stop_requested != nullptr) *_stop_requested = true;
      return MPLAY_OK;
    case MPLAY_CMD_RESTART:
      _player->restart();
      return MPLAY_OK;
    case MPLAY_CMD_PREVIOUS:
      return MPLAY_UNSUPPORTED;
    case MPLAY_CMD_NEXT:
      if (_next_requested != nullptr) *_next_requested = true;
      return MPLAY_OK;
    case MPLAY_CMD_SET_LOOP: {
      const bool enabled = value != 0;
      if (_loop_flag != nullptr) *_loop_flag = enabled;
      _player->setLoop(enabled);
      return MPLAY_OK;
    }
    case MPLAY_CMD_SET_VOLUME: {
      int32_t v = value;
      if (v < 0) v = 0;
      if (v > 127) v = 127;
      midi_engine_set_master_volume(_engine, static_cast<uint8_t>(v));
      return MPLAY_OK;
    }
    case MPLAY_CMD_ADJUST_VOLUME: {
      int32_t v = static_cast<int32_t>(midi_engine_get_master_volume(_engine)) + value;
      if (v < 0) v = 0;
      if (v > 127) v = 127;
      midi_engine_set_master_volume(_engine, static_cast<uint8_t>(v));
      return MPLAY_OK;
    }
    case MPLAY_CMD_SEEK_REL_MS:
      return MPLAY_UNSUPPORTED;
    default:
      return MPLAY_UNSUPPORTED;
  }
}
