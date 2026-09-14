#include "midi_playback_api.h"

mplay_result_t mplay_get_snapshot(const mplay_api_t *api, mplay_snapshot_t *out)
{
    if (api == NULL || out == NULL || api->get_snapshot == NULL) return MPLAY_INVALID;
    return api->get_snapshot(api->user, out);
}

mplay_result_t mplay_send_command(const mplay_api_t *api, mplay_command_t command, int32_t value)
{
    if (api == NULL || api->command == NULL) return MPLAY_INVALID;
    return api->command(api->user, command, value);
}

int mplay_has_capability(const mplay_api_t *api, uint32_t capability)
{
    return api != NULL && (api->capabilities & capability) == capability;
}
