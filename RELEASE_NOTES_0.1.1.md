# MidMod 0.1.1

- Enable SdFat UTF-8 long filenames (`USE_UTF8_LONG_NAMES=1`).
- Japanese MIDI/folder names are handled as UTF-8 from SD listing through UI/path use.
- Add configurable sample SAM2695 UART GPIO (`MIDMOD_SYNTH_RX_PIN`, `MIDMOD_SYNTH_TX_PIN`).
- Add `docs/CSV_PROFILES.md` with profile creation/reference.
- Add `docs/SAM2695_UART.md` and `examples/sam2695_uart_backend.cpp`.
- Correct API quick-start backend callback to the actual buffer callback signature.
