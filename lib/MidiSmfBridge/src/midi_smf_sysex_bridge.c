#include "midi_smf_sysex_bridge.h"

void midi_smf_sysex_bridge_reset(midi_smf_sysex_bridge_t *b)
{
    if (b == 0) return;
    b->open_track = -1;
    b->forced_resyncs = 0u;
}

void midi_smf_sysex_bridge_feed(midi_smf_sysex_bridge_t *b,
                                uint8_t track,
                                const uint8_t *data,
                                size_t size,
                                midi_smf_emit_byte_fn emit_byte,
                                void *emit_user)
{
    size_t i = 0u;
    uint8_t first;

    if (b == 0 || data == 0 || size == 0u || emit_byte == 0) return;
    first = data[0];

    if (b->open_track >= 0 && b->open_track != (int16_t)track) {
        emit_byte(emit_user, 0xf7u);
        b->open_track = -1;
        ++b->forced_resyncs;
    }

    if (b->open_track < 0) {
        if (first == 0xf0u) {
            emit_byte(emit_user, 0xf0u);
            b->open_track = (int16_t)track;
            i = 1u;
        } else if ((first & 0x80u) != 0u) {
            for (i = 0u; i < size; ++i) emit_byte(emit_user, data[i]);
            return;
        } else {
            emit_byte(emit_user, 0xf0u);
            b->open_track = (int16_t)track;
        }
    } else if (first == 0xf0u) {
        emit_byte(emit_user, 0xf7u);
        ++b->forced_resyncs;
        emit_byte(emit_user, 0xf0u);
        b->open_track = (int16_t)track;
        i = 1u;
    }

    for (; i < size; ++i) {
        const uint8_t v = data[i];
        emit_byte(emit_user, v);
        if (v == 0xf7u) {
            b->open_track = -1;
            return;
        }
    }
}
