#ifndef MIDI_PLAYBACK_API_H
#define MIDI_PLAYBACK_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * UI/host boundary for a MIDI playback or emulation source.
 *
 * This API intentionally knows nothing about GS2SAM, SAM2695, SMF, Arduino,
 * PX68 or any particular display. A UI can consume this interface, while a
 * player/emulator implements it.
 */

#define MPLAY_CHANNELS 16u
#define MPLAY_SPECTRUM_BINS 24u
#define MPLAY_TITLE_MAX 64u
#define MPLAY_PROFILE_MAX 48u

typedef enum mplay_state {
    MPLAY_STOPPED = 0,
    MPLAY_PLAYING = 1,
    MPLAY_PAUSED  = 2
} mplay_state_t;

typedef struct mplay_snapshot {
    char title[MPLAY_TITLE_MAX];
    char profile[MPLAY_PROFILE_MAX];
    mplay_state_t state;
    uint8_t master_volume;             /* 0..127 */
    uint8_t loop_enabled;              /* boolean */
    uint16_t tempo_bpm;
    uint32_t elapsed_ms;
    uint32_t total_ms;                 /* 0 = unknown/live */
    uint8_t channel_activity[MPLAY_CHANNELS]; /* 0..127 */
    uint8_t spectrum[MPLAY_SPECTRUM_BINS];   /* 0..127 */
} mplay_snapshot_t;

typedef enum mplay_command {
    MPLAY_CMD_PLAY_PAUSE = 0,
    MPLAY_CMD_STOP,
    MPLAY_CMD_RESTART,
    MPLAY_CMD_PREVIOUS,
    MPLAY_CMD_NEXT,
    MPLAY_CMD_SEEK_REL_MS,
    MPLAY_CMD_SET_LOOP,
    MPLAY_CMD_SET_VOLUME,
    MPLAY_CMD_ADJUST_VOLUME
} mplay_command_t;

enum {
    MPLAY_CAP_PLAY_PAUSE    = 1u << 0,
    MPLAY_CAP_STOP          = 1u << 1,
    MPLAY_CAP_RESTART       = 1u << 2,
    MPLAY_CAP_PREVIOUS      = 1u << 3,
    MPLAY_CAP_NEXT          = 1u << 4,
    MPLAY_CAP_SEEK_REL      = 1u << 5,
    MPLAY_CAP_LOOP          = 1u << 6,
    MPLAY_CAP_VOLUME        = 1u << 7,
    MPLAY_CAP_CHANNEL_METER = 1u << 8,
    MPLAY_CAP_SPECTRUM      = 1u << 9
};

typedef enum mplay_result {
    MPLAY_OK = 0,
    MPLAY_UNSUPPORTED = 1,
    MPLAY_INVALID = 2,
    MPLAY_ERROR = -1
} mplay_result_t;

typedef mplay_result_t (*mplay_get_snapshot_fn)(void *user, mplay_snapshot_t *out);
typedef mplay_result_t (*mplay_command_fn)(void *user, mplay_command_t command, int32_t value);

typedef struct mplay_api {
    uint32_t capabilities;
    mplay_get_snapshot_fn get_snapshot;
    mplay_command_fn command;
    void *user;
} mplay_api_t;

mplay_result_t mplay_get_snapshot(const mplay_api_t *api, mplay_snapshot_t *out);
mplay_result_t mplay_send_command(const mplay_api_t *api, mplay_command_t command, int32_t value);
int mplay_has_capability(const mplay_api_t *api, uint32_t capability);

#ifdef __cplusplus
}
#endif

#endif /* MIDI_PLAYBACK_API_H */
