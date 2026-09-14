# MidMod API

MidMod keeps the reusable boundary on the **MIDI side**. The Cardputer UI is only a sample.

## 1. Feed MIDI

For an emulator such as PX68, feed each byte emitted by the emulated MIDI UART:

```c
midi_engine_feed_byte(&engine, midi_byte);
```

A host that already has a buffer may use:

```c
midi_engine_feed_buffer(&engine, bytes, len);
```

A host with parsed channel messages may use:

```c
midi_engine_feed_channel(&engine, status, data, data_len);
```

## 2. Backend = physical output

```c
static void midiWrite(void *user, const uint8_t *bytes, size_t len)
{
    /* UART, USB MIDI, BLE MIDI, queue, etc. */
}

static void setVolume(void *user, uint8_t volume)
{
    /* optional target master-volume hook */
}

midi_engine_backend_t backend = {
    .write = midiWrite,
    .set_master_volume = setVolume,
    .user = NULL,
};

midi_engine_t engine;
midi_engine_init(&engine, &backend, 32);
```

`MidiEngine` does not know UART numbers or GPIO pins. Those belong to the backend/host.

## 3. Pass-through or processor

Without a processor, input is pass-through.

For GS → SAM2695:

```c
gs2sam_processor_t gs;
gs2sam_processor_init_sc55mk2(&gs, &engine, 88);

midi_processor_t processor = gs2sam_processor_interface(&gs);
midi_engine_set_processor(&engine, &processor);
```

A future processor can replace this without changing the source-side API.

## 4. SAM2695 UART sample

Cardputer sample:

```cpp
midmodSampleInitSynthUart(MIDMOD_SYNTH_RX_PIN, MIDMOD_SYNTH_TX_PIN);
```

The helper calls:

```cpp
synth.begin(&Serial2, UNIT_SYNTH_BAUD, rxPin, txPin);
```

Change the sample GPIO with PlatformIO build flags:

```ini
-DMIDMOD_SYNTH_RX_PIN=13
-DMIDMOD_SYNTH_TX_PIN=14
```

See `docs/SAM2695_UART.md` and `examples/sam2695_uart_backend.cpp`.

## 5. CSV profile

Parse a profile and apply it only to `Gs2SamProcessor`:

```c
gs2sam_processor_apply_csv(&gs, &profile);
midi_engine_set_profile_label(&engine, profile.comment);
```

The generic engine does not know SC-55, SC-88Pro, or SAM2695.

CSV format: `docs/CSV_PROFILES.md`.

## 6. Snapshot (optional UI)

```c
midi_engine_snapshot_t s;
midi_engine_get_snapshot(&engine, &s);
```

The snapshot exposes common state plus MIDI-derived channel activity and spectrum data.

For transport controls, use `MidiPlaybackApi` (`mplay_api_t`). A headless emulator does not need it.

## 7. PX68 pattern

```c
void px68_midi_tx(uint8_t byte)
{
    midi_engine_feed_byte(&engine, byte);
}
```

The backend should normally enqueue physical output rather than block the guest CPU.

See `examples/px68_midi_engine_bridge.c`.
