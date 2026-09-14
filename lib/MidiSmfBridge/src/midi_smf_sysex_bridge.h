#ifndef MIDI_SMF_SYSEX_BRIDGE_H
#define MIDI_SMF_SYSEX_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct midi_smf_sysex_bridge {
    int16_t open_track;
    uint32_t forced_resyncs;
} midi_smf_sysex_bridge_t;

typedef void (*midi_smf_emit_byte_fn)(void *user, uint8_t byte);

void midi_smf_sysex_bridge_reset(midi_smf_sysex_bridge_t *b);
void midi_smf_sysex_bridge_feed(midi_smf_sysex_bridge_t *b,
                                uint8_t track,
                                const uint8_t *data,
                                size_t size,
                                midi_smf_emit_byte_fn emit_byte,
                                void *emit_user);

#ifdef __cplusplus
}
#endif

#endif /* MIDI_SMF_SYSEX_BRIDGE_H */
