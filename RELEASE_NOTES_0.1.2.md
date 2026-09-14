# MidMod 0.1.2

Small Cardputer playback UI fix.

- Spectrum analyser display now downsamples the 24-bin API snapshot to 12 display bars.
- Bars have wider spacing for better readability on the 240x135 display.
- The analyser has an explicit complete outer frame; the right edge is kept clear of the bars.
- Graph height is slightly reduced and moved inward.
- No MidiEngine, processor, CSV profile, playback API, or MIDI timing behavior changed.

The underlying `MPLAY_SPECTRUM_BINS` remains 24, so other front ends (including future PX68/Tab5 UIs) retain the original spectrum resolution.
