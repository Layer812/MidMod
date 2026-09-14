#ifndef SMF_PLAYBACK_API_H
#define SMF_PLAYBACK_API_H

#include <midi_playback_api.h>
#include <midi_engine.h>
#include "smf_playback.h"

/* Cardputer/SMF implementation of the generic playback boundary.
 * The UI never needs to include SmfPlayback or target-processor headers. */
class SmfPlaybackApi {
public:
  void begin(SmfPlayback *player,
             midi_engine_t *engine,
             bool *loop_flag,
             bool *next_requested,
             bool *stop_requested);

  const mplay_api_t *api() const { return &_api; }

private:
  static mplay_result_t snapshotThunk(void *user, mplay_snapshot_t *out);
  static mplay_result_t commandThunk(void *user, mplay_command_t command, int32_t value);
  mplay_result_t getSnapshot(mplay_snapshot_t *out) const;
  mplay_result_t command(mplay_command_t command, int32_t value);

  SmfPlayback *_player = nullptr;
  midi_engine_t *_engine = nullptr;
  bool *_loop_flag = nullptr;
  bool *_next_requested = nullptr;
  bool *_stop_requested = nullptr;
  mplay_api_t _api{};
};

#endif
