#ifndef SMF_PLAYBACK_H
#define SMF_PLAYBACK_H

#include <Arduino.h>
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include <midi_engine.h>
#include <midi_smf_sysex_bridge.h>

#include "smf_probe.h"

class SmfPlayback {
public:
  enum PollResult { IDLE, RUNNING, ENDED, ERROR };

  void begin(SdFat *sd, midi_engine_t *engine);
  int load(const char *path);
  PollResult poll();
  void stop();
  void restart();
  void setPaused(bool paused);
  void togglePaused();
  void setLoop(bool enabled);

  bool isLoaded() const { return _loaded; }
  bool isPaused() const { return _paused; }
  bool isLooping() const { return _loop; }
  uint32_t totalMs() const { return _probe.total_ms; }
  uint32_t elapsedMs() const;
  uint8_t trackCount() const { return (uint8_t)_probe.tracks; }
  uint16_t tempo() { return _smf.getTempo(); }
  const char *path() const { return _path; }
  uint32_t sysexResyncs() const { return _sysex_bridge.forced_resyncs; }

private:
  static SmfPlayback *_active;
  static void midiThunk(midi_event *pev);
  static void sysexThunk(sysex_event *pev);
  static void engineFeedByte(void *user, uint8_t byte);
  void onMidi(midi_event *pev);
  void onSysex(sysex_event *pev);
  void allSoundOff();
  void publishTimeline();

  SdFat *_sd = nullptr;
  midi_engine_t *_engine = nullptr;
  MD_MIDIFile _smf;
  midi_smf_sysex_bridge_t _sysex_bridge{};
  SmfProbeInfo _probe{};
  char _path[256]{};
  bool _loaded = false;
  bool _paused = false;
  bool _loop = false;
  uint32_t _start_ms = 0;
  uint32_t _pause_start_ms = 0;
  uint32_t _paused_ms = 0;
};

#endif
