from pathlib import Path
import csv, sys
root = Path(__file__).resolve().parents[1]
profdir = root / 'sdcard'
expected = {
    'sc55mk2.csv': {'tone':18, 'drumkit':10, 'drumnote':7, 'config':9},
    'sc88pro.csv': {'tone':50, 'drumkit':10, 'drumnote':7, 'config':9},
}
for name, want in expected.items():
    p = profdir / name
    if not p.exists():
        raise SystemExit(f'FAIL missing {p}')
    counts = {k:0 for k in want}
    all_lines = p.read_text(encoding='utf-8').splitlines()
    first = all_lines[0]
    if not first.startswith('# ') or len(first[2:].strip()) == 0:
        raise SystemExit(f'FAIL {name} first line must be a non-empty # screen comment')
    print(f'PASS {name} screen comment: {first[2:].strip()}')
    if '# GS2SAM_PROFILE,1' not in all_lines:
        raise SystemExit(f'FAIL {name} missing # GS2SAM_PROFILE,1 signature')
    with p.open(encoding='utf-8', newline='') as f:
        for row in csv.reader(line for line in f if not line.lstrip().startswith('#')):
            if not row or not row[0].strip():
                continue
            typ = row[0].strip()
            if typ in counts:
                counts[typ] += 1
    if counts != want:
        raise SystemExit(f'FAIL {name} counts {counts}, expected {want}')
    print(f'PASS {name} {counts}')
print('profile CSV validation: PASS')
