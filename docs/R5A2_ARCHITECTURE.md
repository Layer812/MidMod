# R5A2 architecture change

R5A1 still embedded GS2SAM directly inside the lower engine structure. R5A2
removes that target-specific dependency.

- `MidiEngine` — generic input observer, output backend, volume/reset handling,
  pass-through mode and visualization state.
- `Gs2SamProcessor` — GS semantic retargeting + CSV profiles for SAM2695.
- `MidiSmfBridge` — generic split-SysEx reconstruction for SMF callback sources.
- `MidiPlaybackApi` — optional UI/control boundary; unchanged.
- `src/ui/cardputer_ui_sample.*` — sample UI only.

This means a modern MIDI module can use `MidiEngine` in pass-through mode or via
a future processor without carrying GS2SAM/SAM2695 semantics into the generic
API.
