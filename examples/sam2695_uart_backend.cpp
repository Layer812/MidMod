/* MidMod sample: SAM2695 UART backend with selectable GPIO pins.
 * This file is documentation/example code; Cardputer's actual sample lives in src/main.cpp.
 */
#include <Arduino.h>
#include <M5UnitSynth.h>
#include <midi_engine.h>

static M5UnitSynth synth;
static HardwareSerial *midiSerial = &Serial2;

static void midiWrite(void *, const uint8_t *bytes, size_t len)
{
    midiSerial->write(bytes, len);
}

static void setMasterVolume(void *, uint8_t value)
{
    synth.setMasterVolume(value);
}

static void initSam2695(midi_engine_t *engine, int rxPin, int txPin)
{
    synth.begin(midiSerial, UNIT_SYNTH_BAUD, rxPin, txPin);

    midi_engine_backend_t backend = {
        midiWrite,
        setMasterVolume,
        nullptr,
    };
    midi_engine_init(engine, &backend, 32);
}

/* Example:
 *   midi_engine_t engine;
 *   initSam2695(&engine, 1, 2);     // Cardputer
 *   // initSam2695(&engine, 13, 14); // another host
 */
