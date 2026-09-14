#include "smf_playback.h"

#include <string.h>

SmfPlayback *SmfPlayback::_active = nullptr;

static const char *baseNameOf(const char *path)
{
  if (path == nullptr) return "";
  const char *p = strrchr(path, '/');
  return p != nullptr ? p + 1 : path;
}

void SmfPlayback::begin(SdFat *sd, midi_engine_t *engine)
{
  _sd = sd;
  _engine = engine;
  _active = this;
  _smf.begin(sd);
  _smf.setMidiHandler(midiThunk);
  _smf.setSysexHandler(sysexThunk);
  midi_smf_sysex_bridge_reset(&_sysex_bridge);
}

int SmfPlayback::load(const char *path)
{
  if (_sd == nullptr || _engine == nullptr || path == nullptr) return -1;
  if (_loaded) stop();

  strncpy(_path, path, sizeof(_path) - 1u);
  _path[sizeof(_path) - 1u] = '\0';
  memset(&_probe, 0, sizeof(_probe));
  (void)smfProbeFile(*_sd, _path, _probe);

  midi_engine_reset(_engine);
  midi_smf_sysex_bridge_reset(&_sysex_bridge);
  midi_engine_set_title(_engine, baseNameOf(_path));
  midi_engine_set_timeline(_engine, 0u, _probe.total_ms);
  if (_probe.initial_bpm != 0u) midi_engine_set_tempo(_engine, _probe.initial_bpm);

  const int err = _smf.load(_path);
  if (err != MD_MIDIFile::E_OK) {
    midi_engine_set_play_state(_engine, MIDI_ENGINE_STOPPED);
    return err;
  }

  _smf.looping(_loop);
  _loaded = true;
  _paused = false;
  _start_ms = (uint32_t)millis();
  _pause_start_ms = 0;
  _paused_ms = 0;
  midi_engine_set_loop(_engine, _loop ? 1u : 0u);
  midi_engine_set_play_state(_engine, MIDI_ENGINE_PLAYING);
  publishTimeline();
  return MD_MIDIFile::E_OK;
}

uint32_t SmfPlayback::elapsedMs() const
{
  if (!_loaded) return 0;
  uint32_t now = _paused ? _pause_start_ms : (uint32_t)millis();
  uint32_t elapsed = now - _start_ms;
  if (elapsed >= _paused_ms) elapsed -= _paused_ms;
  else elapsed = 0;
  if (_loop && _probe.total_ms != 0u) elapsed %= _probe.total_ms;
  if (!_loop && _probe.total_ms != 0u && elapsed > _probe.total_ms) elapsed = _probe.total_ms;
  return elapsed;
}

void SmfPlayback::publishTimeline()
{
  if (_engine == nullptr) return;
  midi_engine_set_timeline(_engine, elapsedMs(), _probe.total_ms);
  midi_engine_set_tempo(_engine, _smf.getTempo());
}

SmfPlayback::PollResult SmfPlayback::poll()
{
  if (!_loaded) return IDLE;
  publishTimeline();
  if (_paused) return RUNNING;

  if (!_smf.isEOF()) {
    (void)_smf.getNextEvent();
    publishTimeline();
    return RUNNING;
  }

  if (_loop) {
    _smf.restart();
    midi_engine_reset(_engine);
    midi_smf_sysex_bridge_reset(&_sysex_bridge);
    _start_ms = (uint32_t)millis();
    _paused_ms = 0;
    midi_engine_set_play_state(_engine, MIDI_ENGINE_PLAYING);
    return RUNNING;
  }
  return ENDED;
}

void SmfPlayback::allSoundOff()
{
  if (_engine == nullptr) return;
  const uint8_t data[2] = {120u, 0u};
  for (uint8_t ch = 0; ch < 16u; ++ch)
    midi_engine_feed_channel(_engine, (uint8_t)(0xb0u | ch), data, 2u);
}

void SmfPlayback::stop()
{
  if (_loaded) {
    _smf.close();
    allSoundOff();
  }
  _loaded = false;
  _paused = false;
  midi_engine_set_play_state(_engine, MIDI_ENGINE_STOPPED);
}

void SmfPlayback::restart()
{
  if (!_loaded) return;
  allSoundOff();
  _smf.restart();
  midi_engine_reset(_engine);
  midi_smf_sysex_bridge_reset(&_sysex_bridge);
  _start_ms = (uint32_t)millis();
  _paused_ms = 0;
  if (_paused) _pause_start_ms = _start_ms;
  publishTimeline();
}

void SmfPlayback::setPaused(bool paused)
{
  if (!_loaded || paused == _paused) return;
  const uint32_t now = (uint32_t)millis();
  if (paused) {
    _smf.pause(true);
    allSoundOff();
    _pause_start_ms = now;
    _paused = true;
    midi_engine_set_play_state(_engine, MIDI_ENGINE_PAUSED);
  } else {
    _paused_ms += now - _pause_start_ms;
    _smf.pause(false);
    _paused = false;
    midi_engine_set_play_state(_engine, MIDI_ENGINE_PLAYING);
  }
  publishTimeline();
}

void SmfPlayback::togglePaused() { setPaused(!_paused); }

void SmfPlayback::setLoop(bool enabled)
{
  _loop = enabled;
  _smf.looping(enabled);
  if (_engine != nullptr) midi_engine_set_loop(_engine, enabled ? 1u : 0u);
}

void SmfPlayback::midiThunk(midi_event *pev)
{
  if (_active != nullptr) _active->onMidi(pev);
}

void SmfPlayback::sysexThunk(sysex_event *pev)
{
  if (_active != nullptr) _active->onSysex(pev);
}

void SmfPlayback::onMidi(midi_event *pev)
{
  if (_engine == nullptr || pev == nullptr || pev->size == 0u) return;
  const uint8_t base = pev->data[0] & 0xf0u;
  if (base < 0x80u || base > 0xe0u) return;
  const uint8_t status = (uint8_t)(base | (pev->channel & 0x0fu));
  const uint8_t *data = pev->size > 1u ? &pev->data[1] : nullptr;
  midi_engine_feed_channel(_engine, status, data, pev->size > 0u ? (uint8_t)(pev->size - 1u) : 0u);
}

void SmfPlayback::engineFeedByte(void *user, uint8_t byte)
{
  midi_engine_t *engine = static_cast<midi_engine_t *>(user);
  if (engine != nullptr) midi_engine_feed_byte(engine, byte);
}

void SmfPlayback::onSysex(sysex_event *pev)
{
  if (_engine == nullptr || pev == nullptr || pev->size == 0u) return;
  midi_smf_sysex_bridge_feed(&_sysex_bridge, pev->track, pev->data, pev->size,
                             engineFeedByte, _engine);
}
