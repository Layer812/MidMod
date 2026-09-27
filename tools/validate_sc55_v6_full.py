from pathlib import Path
import csv

root = Path(__file__).resolve().parents[1]
p = root / 'sdcard' / 'sc55mk2.csv'
rows = list(csv.reader(p.read_text(encoding='utf-8-sig').splitlines()))
tones = [r for r in rows if r and r[0] == 'tone']
families = [r for r in rows if r and r[0] == 'family']
drumkits = [r for r in rows if r and r[0] == 'drumkit']
drumnotes = [r for r in rows if r and r[0] == 'drumnote']

if len(tones) != 220:
    raise SystemExit(f'FAIL tone count {len(tones)} != 220')
mt32 = [r for r in tones if r[1] == '127']
variations = [r for r in tones if r[1] != '127']
if len(mt32) != 128 or len(variations) != 92:
    raise SystemExit(f'FAIL split mt32={len(mt32)} variation={len(variations)}')
if len(families) != 12 or len(drumkits) != 10 or len(drumnotes) != 7:
    raise SystemExit('FAIL family/drum counts')

keys = [(r[1], r[2], r[3]) for r in tones]
if len(set(keys)) != len(keys):
    raise SystemExit('FAIL duplicate tone source keys')

# Bank-127 section must cover every source program exactly once.
pcs = sorted(int(r[3]) for r in mt32)
if pcs != list(range(1, 129)):
    raise SystemExit('FAIL MT-32 bank does not cover programs 1..128 exactly once')

expect = {
    ('127','*','4'): ('127','5'),   # Elec Piano 1 -> E.Piano1
    ('127','*','40'): ('0','54'),  # Funny Vox -> GM Voice Oohs
    ('8','*','5'): ('127','4'),    # Detuned EP1
    ('8','*','63'): ('127','27'),  # Synth Brass3
}
actual = {(r[1],r[2],r[3]): (r[4],r[5]) for r in tones}
for k, v in expect.items():
    if actual.get(k) != v:
        raise SystemExit(f'FAIL mapping {k}: {actual.get(k)} != {v}')

print('SC55 V6 full profile validation: PASS')
print('tone=220 mt32=128 variation=92 family=12 drumkit=10 drumnote=7 unique=220')
