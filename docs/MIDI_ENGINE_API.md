# Generic MIDI Engine API (R5A2)

The reusable lower layer is intentionally **name-neutral**. `MidMod` is the release/project name. The public C API intentionally remains generic (`midi_engine_*`, `mplay_*`) so applications are not tied to branding.

The engine has no Arduino, display, filesystem, M5Stack, SAM2695 or SMF
dependency.

## Architecture

```text
source / emulator
(SMF, PX68, live MIDI, ...)
        |
        v
   MidiEngine                <- generic / target-neutral
        |
        +---- pass-through processor
        |
        +---- GS2SAM processor          <- current SAM2695 target
        |
        +---- future GM2 / GS / XG / modern-device processor
        |
        v
 target MIDI backend
```

`MidiEngine` observes the **source** MIDI stream for 16-channel activity and a
24-bin MIDI-derived spectrum. It then hands each byte to the currently selected
processor. The default processor is lossless pass-through.

## Minimal modern-device / pass-through use

```c
midi_engine_backend_t be = {
    .write = host_midi_write,
    .set_master_volume = host_set_volume,
    .user = host,
};

midi_engine_t engine;
midi_engine_init(&engine, &be, 32);

/* No semantic converter installed: source bytes are preserved exactly. */
midi_engine_feed_byte(&engine, uart_tx_byte);
```

## Install GS2SAM as one processor

```c
gs2sam_processor_t gs;
gs2sam_processor_init_sc55mk2(&gs, &engine, 88);

midi_processor_t p = gs2sam_processor_interface(&gs);
midi_engine_set_processor(&engine, &p);
midi_engine_set_profile_label(&engine, "SC-55mkII / GS");
```

CSV changes only the processor profile:

```c
gs2sam_processor_apply_csv(&gs, &profile);
midi_engine_set_profile_label(&engine, profile.comment);
```

The generic engine does not include `gs2sam.h` and does not know that the target
is SAM2695.

## PX68 boundary

The preferred emulator entry point remains one byte per emulated MIDI-UART TX:

```c
midi_engine_feed_byte(&engine, value);
```

The backend callback should enqueue output to PX68/RetroP4's host MIDI worker.
It should not block the guest CPU on the physical UART.

## Visualization / metadata

```c
midi_engine_snapshot_t s;
midi_engine_get_snapshot(&engine, &s);
```

The snapshot contains title/profile label, play state, elapsed/total time,
tempo, volume, loop state, 16 channel activity values and 24 spectrum bins.
The spectrum is MIDI-note-derived, not an audio FFT, so external hardware synths
remain observable even when their rendered audio does not return to the MCU.

## SMF bridge

`MidiSmfBridge` is also target-neutral. It reconstructs split SMF SysEx/F7
callbacks into raw MIDI bytes and feeds any engine/processor chosen by the host.
