# MidMod CSV Profiles

MidMod の GS → SAM2695 変換定義は、SDカード上のCSVで変更できます。
ファームを再ビルドせず、音色・ドラム・変換方針を追加できます。

---

## 日本語

### 1. 使い方

CSVはSDカードの**ルート**へ置きます。

```text
/sc55mk2.csv
/sc88pro.csv
/my_module.csv
```

起動時は `sc55mk2.csv` を優先します。
ファイラーで `P` を押すと、有効なCSVを順番に切り替えます。

CSVとして認識されるには、次のシグネチャ行が必要です。

```csv
# GS2SAM_PROFILE,1
```

1行目のコメントは画面のプロファイル名として使われます。

```csv
# SC-55mkII / GS
# GS2SAM_PROFILE,1
```

### 2. 最小CSV

```csv
# My GS Profile
# GS2SAM_PROFILE,1
meta,name,My Profile
meta,source,My MIDI Source
meta,target,SAM2695
config,rhythm_volume_percent,88
```

この状態では、明示的な音色変換をほとんど行わず、既定のfallbackを使います。

### 3. tone — 音色変換

形式:

```text
tone,src_msb,src_lsb,src_pc,dst_bank,dst_pc,vib_rate,vib_depth,vib_delay,cutoff,resonance,attack,decay,release,label
```

例:

```csv
tone,8,*,5,127,4,,,,,,,,,Detuned EP 1 -> SAM Detuned EP1
```

意味:

- `src_msb`: 入力Bank Select MSB。0..127
- `src_lsb`: 入力Bank Select LSB。0..127、または `*` で任意
- `src_pc`: 入力Program。**1..128表記**
- `dst_bank`: SAM2695へ送るBank MSB。0..127
- `dst_pc`: SAM2695へ送るProgram。**1..128表記**
- `vib_rate` ～ `release`: 任意の補正値。空欄なら変更なし。指定する場合は -64..63
- `label`: 人間向けメモ。変換処理には使いません

SC-88ProのようにLSBがSound Mapを表す場合は、LSBごとに行を作れます。

```csv
tone,8,1,63,127,27,,,,,,,,,Synth Brass 3 [SC-55 map]
tone,8,2,63,127,27,,,,,,,,,Synth Brass 3 [SC-88 map]
```

### 4. drumkit — ドラムセット変換

形式:

```text
drumkit,src_pc,dst_pc,flags,label
```

Programは1..128表記です。

例:

```csv
drumkit,25,17,0,ELECTRONIC -> POWER
drumkit,57,128,drop_unmapped,SFX sparse mapping
```

`flags`:

- `0`: 通常
- `drop_unmapped`: `drumnote` に定義されていない音を出さない

### 5. drumnote — ドラムノート変換

形式:

```text
drumnote,src_pc,src_note,dst_note,label
```

例:

```csv
drumnote,25,38,40,Electronic Snare -> SAM Snare
drumnote,57,70,94,SFX Helicopter -> CM Helicopter
```

- `src_pc`: 元ドラムセットProgram。1..128
- `src_note`: 元MIDI Note。0..127
- `dst_note`: 出力MIDI Note。0..127、または `drop`

### 6. config — 変換方針

現在使える主な項目:

```csv
config,rhythm_volume_percent,88
config,pass_unknown_sysex,1
config,pass_malformed_sysex,1
config,capital_tone_fallback,1
config,drum_fallback,1
config,drum_retarget,1
config,emulate_shared_drum_maps,1
config,emulate_drum_rx,1
config,emulate_key_range,1
```

bool値は `1/0`, `true/false`, `on/off`, `yes/no` を使えます。

`rhythm_volume_percent` は0..200です。現在の付属profileは88%です。

### 7. meta — 説明情報

```csv
meta,name,SC-88Pro
meta,source,Roland SC-88Pro GS
meta,target,SAM2695
meta,status,curated test map
```

`name/source/target/status` は説明用です。

### 8. 作り方のおすすめ

新しい音源profileは、まず既存CSVをコピーしてください。

```text
sc88pro.csv -> my_module.csv
```

そして次の順番で増やすのが安全です。

1. 1行目の表示名と `meta` を変更
2. `tone` を数個だけ追加
3. 実際のSMFで聴き比べ
4. 必要な音色だけ追加
5. ドラム差が気になる場合だけ `drumkit` / `drumnote` を追加

大量の推測mappingを一度に入れるより、確認できたmappingだけ増やす方針を推奨します。

### 9. エラー時

認識済み行に不正な数値や形式があるCSVは、途中まで適用せず**profile全体を拒否**します。
シグネチャがないCSVもprofileとして採用しません。
未知の行typeは将来互換のため無視します。

付属のチェック:

```bat
python tools\validate_profiles.py
```

---

## English

Profiles live in the SD-card root and require this signature:

```csv
# GS2SAM_PROFILE,1
```

The first comment line is the on-screen profile label.

### Tone rule

```text
tone,src_msb,src_lsb,src_pc,dst_bank,dst_pc,vib_rate,vib_depth,vib_delay,cutoff,resonance,attack,decay,release,label
```

- bank values: 0..127
- program values: human-readable 1..128
- `src_lsb`: 0..127 or `*`
- optional modifiers: -64..63
- label: documentation only

### Drum kit rule

```text
drumkit,src_pc,dst_pc,flags,label
```

`flags` is `0` or `drop_unmapped`.

### Drum note rule

```text
drumnote,src_pc,src_note,dst_note,label
```

`dst_note` may be 0..127 or `drop`.

### Config

Supported keys include `rhythm_volume_percent`, `pass_unknown_sysex`, `pass_malformed_sysex`, `capital_tone_fallback`, `drum_fallback`, `drum_retarget`, `emulate_shared_drum_maps`, `emulate_drum_rx`, and `emulate_key_range`.

A recognized malformed row rejects the complete profile. Unknown row types are ignored for forward compatibility.
