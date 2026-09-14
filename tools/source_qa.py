from pathlib import Path
import sys

root = Path(__file__).resolve().parents[1]
main = (root / 'src/main.cpp').read_text(encoding='utf-8')
ui = (root / 'src/ui/cardputer_ui_sample.cpp').read_text(encoding='utf-8')
pio = (root / 'platformio.ini').read_text(encoding='utf-8')
eng_h = (root / 'lib/MidiEngine/src/midi_engine.h').read_text(encoding='utf-8')
proc_h = (root / 'lib/Gs2SamProcessor/src/gs2sam_processor.h').read_text(encoding='utf-8')

checks = {
    'MidMod runtime branding': 'MidMod 0.1.1 - Modifiable MIDI Module' in main,
    'MidMod screen mark': 'g_canvas.print("MidMod")' in ui,
    'generic MidiEngine': (root/'lib/MidiEngine/src/midi_engine.c').exists(),
    'engine independent of GS2SAM': 'gs2sam' not in eng_h.lower(),
    'processor API': 'midi_engine_set_processor' in eng_h and 'midi_processor_t' in eng_h,
    'GS2SAM processor': 'gs2sam_processor_interface' in proc_h,
    'playback API': (root/'lib/MidiPlaybackApi/src/midi_playback_api.c').exists(),
    'Japanese UI support': 'efontJA_10' in ui,
    'SdFat UTF-8 LFN enabled': '-DUSE_UTF8_LONG_NAMES=1' in pio,
    'sample UART pins configurable': 'MIDMOD_SYNTH_RX_PIN' in main and 'MIDMOD_SYNTH_TX_PIN' in main and 'midmodSampleInitSynthUart' in main,
    'SC55 CSV': (root/'sdcard/sc55mk2.csv').exists(),
    'SC88Pro CSV': (root/'sdcard/sc88pro.csv').exists(),
    '8MB MidMod partition selected': 'partitions_midmod_8mb.csv' in pio,
    '6MB app partition file': '0x600000' in (root/'partitions_midmod_8mb.csv').read_text(encoding='utf-8'),
    'COM5 default': 'upload_port = COM5' in pio and 'monitor_port = COM5' in pio,
    'README Japanese + English': '## 日本語' in (root/'README.md').read_text(encoding='utf-8') and '# English' in (root/'README.md').read_text(encoding='utf-8'),
    'API quick guide': (root/'docs/API.md').exists(),
}

for name, ok in checks.items():
    print(('PASS' if ok else 'FAIL'), name)
if not all(checks.values()):
    sys.exit(1)
