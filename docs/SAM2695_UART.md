# SAM2695 UART / GPIO

MidMod core (`MidiEngine`) does **not** own a UART or GPIO.
The host sample chooses the physical MIDI output pins.

## Cardputer sample default

```text
RX = GPIO 1
TX = GPIO 2
Baud = 31250
```

The sample initializes Unit Synth here:

```cpp
midmodSampleInitSynthUart(MIDMOD_SYNTH_RX_PIN, MIDMOD_SYNTH_TX_PIN);
```

and the helper is intentionally simple:

```cpp
static void midmodSampleInitSynthUart(int rxPin, int txPin)
{
    synth.begin(&Serial2, UNIT_SYNTH_BAUD, rxPin, txPin);
}
```

## Change pins for another machine

Add/replace these PlatformIO build flags:

```ini
build_flags =
    -DMIDMOD_SYNTH_RX_PIN=13
    -DMIDMOD_SYNTH_TX_PIN=14
```

No change to `MidiEngine` or `Gs2SamProcessor` is required.

If the host does not use M5UnitSynth at all, replace only the backend `write` callback with that machine's MIDI UART/USB/BLE output.

## Why this belongs outside MidiEngine

The reusable boundary is:

```text
MIDI source -> MidiEngine -> processor -> backend -> physical transport
```

GPIO/UART selection is a backend/host concern. This keeps the same MidMod processing code usable from Cardputer, PX68, Tab5, USB MIDI, or another future target.
