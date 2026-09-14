# MidiPlaybackApi (R5A1)

`MidiPlaybackApi` is the UI/control boundary. It is intentionally independent of
SMF, PX68, GS2SAM, SAM2695, Arduino, M5Stack and any display implementation.

The project name is MidMod. The C API intentionally remains name-neutral so the engine can be embedded independently of the product UI.

## Direction

```text
MIDI source / emulator                     UI sample
(SMF, PX68, live MIDI, ...)                    |
        |                                      |
        v                                      v
  MIDI semantic core  <---------->  MidiPlaybackApi
        |
        v
  target MIDI backend
```

There are two independent boundaries:

1. Raw/parsed MIDI enters the semantic core (`midi_engine_feed_byte()` or
   `midi_engine_feed_channel()`).
2. A screen or controller talks only through `mplay_api_t`.

## Screen-side interface

```c
mplay_snapshot_t s;
if (mplay_get_snapshot(api, &s) == MPLAY_OK) {
    draw_title(s.title);
    draw_progress(s.elapsed_ms, s.total_ms);
    draw_spectrum(s.spectrum, MPLAY_SPECTRUM_BINS);
}
```

The snapshot contains:

- title and source-profile label
- stopped / playing / paused
- elapsed and total time (`total_ms == 0` is valid for a live source)
- tempo
- master volume
- loop state
- 16-channel activity
- 24-bin MIDI-derived spectrum

## Commands

```c
mplay_send_command(api, MPLAY_CMD_PLAY_PAUSE, 0);
mplay_send_command(api, MPLAY_CMD_RESTART, 0);
mplay_send_command(api, MPLAY_CMD_NEXT, 0);
mplay_send_command(api, MPLAY_CMD_SET_LOOP, 1);
mplay_send_command(api, MPLAY_CMD_ADJUST_VOLUME, -8);
```

Commands are capability-based. A source that cannot seek, for example, simply
omits `MPLAY_CAP_SEEK_REL` and returns `MPLAY_UNSUPPORTED` for
`MPLAY_CMD_SEEK_REL_MS`.

This is important for PX68: emulated software, an SMF player and a live MIDI
source do not necessarily support the same transport operations.

## Cardputer sample implementation

`src/smf/smf_playback_api.*` adapts the Cardputer `SmfPlayback` source and the
MIDI semantic core to `mplay_api_t`.

`src/ui/cardputer_ui_sample.*` depends only on `midi_playback_api.h` for its
playback screen. It does not include the semantic engine header and does not know
about GS2SAM or SAM2695.

The current Cardputer adapter supports:

- play/pause
- stop
- restart current file
- next file request
- loop
- volume
- channel meter and spectrum

Relative seek is deliberately advertised as unsupported until the SMF source can
perform a time-accurate seek without replaying side-effecting MIDI incorrectly.

## PX68 pattern

PX68 can keep its MIDI output path independent:

```c
/* emulated MIDI UART TX */
midi_engine_feed_byte(&midi_engine, byte);
```

If a display is wanted, PX68 exposes its own `mplay_api_t` whose snapshot is
filled from PX68 playback/emulation state plus the MIDI-engine visualization
state. If no screen is wanted, `MidiPlaybackApi` is not required at all.
