/* Skeleton only: demonstrates the boundary expected from PX68.
 * No Cardputer/SMF/display dependency is required. */
#include <stdint.h>
#include <string.h>
#include "midi_playback_api.h"
#include "midi_engine.h"

typedef struct px68_midi_context {
    midi_engine_t *midi;
    uint32_t guest_elapsed_ms;
    uint8_t paused;
} px68_midi_context_t;

static mplay_result_t px68_snapshot(void *user, mplay_snapshot_t *out)
{
    px68_midi_context_t *ctx = (px68_midi_context_t *)user;
    midi_engine_snapshot_t m;
    if (ctx == NULL || ctx->midi == NULL || out == NULL) return MPLAY_INVALID;
    midi_engine_get_snapshot(ctx->midi, &m);
    memset(out, 0, sizeof(*out));
    /* In production copy title/profile/timeline from the PX68 playback owner. */
    out->state = ctx->paused ? MPLAY_PAUSED : MPLAY_PLAYING;
    out->master_volume = m.master_volume;
    out->elapsed_ms = ctx->guest_elapsed_ms;
    memcpy(out->channel_activity, m.channel_activity, MPLAY_CHANNELS);
    memcpy(out->spectrum, m.spectrum, MPLAY_SPECTRUM_BINS);
    return MPLAY_OK;
}

static mplay_result_t px68_command(void *user, mplay_command_t cmd, int32_t value)
{
    px68_midi_context_t *ctx = (px68_midi_context_t *)user;
    if (ctx == NULL) return MPLAY_INVALID;
    switch (cmd) {
    case MPLAY_CMD_PLAY_PAUSE:
        ctx->paused = (uint8_t)!ctx->paused; /* replace with PX68 owner command */
        return MPLAY_OK;
    case MPLAY_CMD_ADJUST_VOLUME: {
        int32_t v = (int32_t)midi_engine_get_master_volume(ctx->midi) + value;
        if (v < 0) v = 0;
        if (v > 127) v = 127;
        midi_engine_set_master_volume(ctx->midi, (uint8_t)v);
        return MPLAY_OK;
    }
    default:
        return MPLAY_UNSUPPORTED;
    }
}

void px68_make_playback_api(px68_midi_context_t *ctx, mplay_api_t *api)
{
    api->capabilities = MPLAY_CAP_PLAY_PAUSE | MPLAY_CAP_VOLUME |
                        MPLAY_CAP_CHANNEL_METER | MPLAY_CAP_SPECTRUM;
    api->get_snapshot = px68_snapshot;
    api->command = px68_command;
    api->user = ctx;
}
