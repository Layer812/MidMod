#include "gs2sam_profile_sc55mk2.h"

/* Built-in SC55 profile for Production V6 mapping.
 * 220 explicit melodic rules:
 *   - 128 Bank-MSB 127 (MT-32-map) program remaps
 *   - 92 documented SC-55/SC-55mkII GS variations
 * Exact tone rules precede native-bank pass-through, then family fallback,
 * then GM Capital Tone fallback. No synthetic NRPN timbre guesses are added.
 *
 * MIDI program numbers below are zero-based. Bank LSB is wildcard here.
 */

static const gs2sam_tone_rule_t k_sc55_rules[] = {
    { 127,   0, 127,   0, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 001 Acou Piano 1 -> Piano 1 [semantic] */
    { 127,   1, 127,   1, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 002 Acou Piano 2 -> Piano 2 [semantic] */
    { 127,   2, 127,   2, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 003 Acou Piano 3 -> Piano 3 [semantic] */
    { 127,   3, 127,   4, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 004 Elec Piano 1 -> E.Piano1 [semantic] */
    { 127,   4, 127,   5, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 005 Elec Piano 2 -> E.Piano2 [semantic] */
    { 127,   5, 127,   4, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 006 Elec Piano 3 -> E.Piano1 [fallback] */
    { 127,   6, 127,   5, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 007 Elec Piano 4 -> E.Piano2 [fallback] */
    { 127,   7, 127,   7, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 008 Honkytonk -> Honky-Tonk [exact] */
    { 127,   8, 127,   8, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 009 Elec Org 1 -> Organ 1 [semantic] */
    { 127,   9, 127,   9, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 010 Elec Org 2 -> Organ 2 [semantic] */
    { 127,  10, 127,  10, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 011 Elec Org 3 -> Organ 3 [semantic] */
    { 127,  11, 127,  10, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 012 Elec Org 4 -> Organ 3 [fallback] */
    { 127,  12, 127,  13, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 013 Pipe Org 1 -> Church Org. [semantic] */
    { 127,  13, 127,  12, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 014 Pipe Org 2 -> Church Org. 2 [semantic] */
    { 127,  14, 127,  13, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 015 Pipe Org 3 -> Church Org. [semantic] */
    { 127,  15, 127,  15, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 016 Accordion -> Accordion Fr. [semantic] */
    { 127,  16, 127,  16, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 017 Harpsi 1 -> Harpsichord [semantic] */
    { 127,  17, 127,  17, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 018 Harpsi 2 -> Coupled Hps. [semantic] */
    { 127,  18, 127,  18, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 019 Harpsi 3 -> Coupled Hps. [semantic] */
    { 127,  19, 127,  19, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 020 Clavi 1 -> Clav. [semantic] */
    { 127,  20, 127,  20, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 021 Clavi 2 -> Clav. [semantic] */
    { 127,  21, 127,  21, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 022 Clavi 3 -> Clav. [semantic] */
    { 127,  22, 127,  22, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 023 Celesta 1 -> Celesta [semantic] */
    { 127,  23, 127,  23, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 024 Celesta 2 -> Celesta [semantic] */
    { 127,  24, 127,  24, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 025 Syn Brass 1 -> Synth Brass1 [exact] */
    { 127,  25, 127,  25, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 026 Syn Brass 2 -> Synth Brass2 [exact] */
    { 127,  26, 127,  26, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 027 Syn Brass 3 -> Synth Brass3 [exact] */
    { 127,  27, 127,  27, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 028 Syn Brass 4 -> Synth Brass4 [exact] */
    { 127,  28, 127,  28, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 029 Syn Bass 1 -> Synth Bass1 [exact] */
    { 127,  29, 127,  29, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 030 Syn Bass 2 -> Synth Bass2 [exact] */
    { 127,  30, 127,  30, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 031 Syn Bass 3 -> Synth Bass3 [exact] */
    { 127,  31, 127,  31, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 032 Syn Bass 4 -> Synth Bass4 [exact] */
    { 127,  32, 127,  32, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 033 Fantasy -> Fantasia [exact] */
    { 127,  33, 127,  43, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 034 Harmo Pan -> Pan Flute [semantic] */
    { 127,  34, 127,  34, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 035 Chorale -> Choir Aahs [semantic] */
    { 127,  35, 127,  35, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 036 Glasses -> Bowed Glass [semantic] */
    { 127,  36, 127,  36, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 037 Soundtrack -> Soundtrack [exact] */
    { 127,  37, 127,  37, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 038 Atmosphere -> Atmosphere [exact] */
    { 127,  38, 127,  40, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 039 Warm Bell -> Tinkle Bell [semantic] */
    { 127,  39,   0,  53, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 040 Funny Vox -> GM Voice Oohs [semantic] */
    { 127,  40, 127,  46, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 041 Echo Bell -> Tubular Bells [semantic] */
    { 127,  41, 127,  41, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 042 Ice Rain -> Ice Rain [exact] */
    { 127,  42, 127,  42, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 043 Oboe 2001 -> Oboe [semantic] */
    { 127,  43, 127,  43, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 044 Echo Pan -> Pan Flute [semantic] */
    { 127,  44, 127,  44, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 045 Doctor Solo -> Saw Wave [semantic] */
    { 127,  45, 127,  45, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 046 School Daze -> Charang [fallback] */
    { 127,  46, 127,  46, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 047 Bellsinger -> Tubular Bells [semantic] */
    { 127,  47, 127,  47, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 048 Square Wave -> Square Wave [exact] */
    { 127,  48, 127,  48, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 049 Str Sect 1 -> Strings [semantic] */
    { 127,  49, 127,  49, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 050 Str Sect 2 -> Tremolo Str. [semantic] */
    { 127,  50, 127,  50, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 051 Str Sect 3 -> Slow Strings [semantic] */
    { 127,  51, 127,  51, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 052 Pizzicato -> Pizzicato Str. [exact] */
    { 127,  52, 127,  52, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 053 Violin 1 -> Violin [semantic] */
    { 127,  53, 127,  52, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 054 Violin 2 -> Violin [semantic] */
    { 127,  54, 127,  54, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 055 Cello 1 -> Cello [semantic] */
    { 127,  55, 127,  54, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 056 Cello 2 -> Cello [semantic] */
    { 127,  56, 127,  56, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 057 Contrabass -> Contrabass [exact] */
    { 127,  57, 127,  57, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 058 Harp 1 -> Harp [semantic] */
    { 127,  58, 127,  57, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 059 Harp 2 -> Harp [semantic] */
    { 127,  59, 127,  59, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 060 Guitar 1 -> Nylon-str. Gt [semantic] */
    { 127,  60, 127,  60, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 061 Guitar 2 -> Steel-Str. Gt [semantic] */
    { 127,  61, 127,  61, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 062 Elec Gtr 1 -> Chorus Gt. [semantic] */
    { 127,  62, 127,  62, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 063 Elec Gtr 2 -> Funk Gt. [semantic] */
    { 127,  63, 127,  63, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 064 Sitar -> Sitar [exact] */
    { 127,  64, 127,  64, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 065 Acou Bass 1 -> Acoustic Bs. [semantic] */
    { 127,  65, 127,  64, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 066 Acou Bass 2 -> Acoustic Bs. [semantic] */
    { 127,  66, 127,  65, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 067 Elec Bass 1 -> Fingered Bs. [semantic] */
    { 127,  67, 127,  66, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 068 Elec Bass 2 -> Picked Bs. [semantic] */
    { 127,  68, 127,  68, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 069 Slap Bass 1 -> Slap Bs. 1 [exact] */
    { 127,  69, 127,  69, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 070 Slap Bass 2 -> Slap Bs. 2 [exact] */
    { 127,  70, 127,  67, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 071 Fretless 1 -> Fretless Bs. [semantic] */
    { 127,  71, 127,  70, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 072 Fretless 2 -> Fretless Bs. [semantic] */
    { 127,  72, 127,  72, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 073 Flute 1 -> Flute [semantic] */
    { 127,  73, 127,  73, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 074 Flute 2 -> Flute [semantic] */
    { 127,  74, 127,  74, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 075 Piccolo 1 -> Piccolo [semantic] */
    { 127,  75, 127,  75, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 076 Piccolo 2 -> Piccolo [semantic] */
    { 127,  76, 127,  76, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 077 Recorder -> Recorder [exact] */
    { 127,  77, 127,  77, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 078 Pan Pipes -> Pan Flute [semantic] */
    { 127,  78, 127,  78, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 079 Sax 1 -> Soprano Sax [semantic] */
    { 127,  79, 127,  79, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 080 Sax 2 -> Alto Sax [semantic] */
    { 127,  80, 127,  80, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 081 Sax 3 -> Tenor Sax [semantic] */
    { 127,  81, 127,  81, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 082 Sax 4 -> Baritone Sax [semantic] */
    { 127,  82, 127,  82, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 083 Clarinet 1 -> Clarinet [semantic] */
    { 127,  83, 127,  83, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 084 Clarinet 2 -> Clarinet [semantic] */
    { 127,  84, 127,  84, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 085 Oboe -> Oboe [exact] */
    { 127,  85, 127,  85, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 086 Engl Horn -> English Horn [semantic] */
    { 127,  86, 127,  86, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 087 Bassoon -> Bassoon [exact] */
    { 127,  87, 127,  87, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 088 Harmonica -> Harmonica [exact] */
    { 127,  88, 127,  88, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 089 Trumpet 1 -> Trumpet [semantic] */
    { 127,  89, 127,  88, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 090 Trumpet 2 -> Trumpet [semantic] */
    { 127,  90, 127,  90, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 091 Trombone 1 -> Trombone [semantic] */
    { 127,  91, 127,  91, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 092 Trombone 2 -> Trombone [semantic] */
    { 127,  92, 127,  92, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 093 Fr Horn 1 -> French Horn [semantic] */
    { 127,  93, 127,  93, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 094 Fr Horn 2 -> French Horn [semantic] */
    { 127,  94, 127,  94, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 095 Tuba -> Tuba [exact] */
    { 127,  95, 127,  95, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 096 Brs Sect 1 -> Brass [semantic] */
    { 127,  96, 127,  96, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 097 Brs Sect 2 -> Brass 2 [semantic] */
    { 127,  97, 127,  97, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 098 Vibe 1 -> Vibraphone [semantic] */
    { 127,  98, 127,  98, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 099 Vibe 2 -> Vibraphone [semantic] */
    { 127,  99, 127,  99, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 100 Syn Mallet -> Kalimba [semantic] */
    { 127, 100, 127, 100, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 101 Windbell -> Tinkle Bell [semantic] */
    { 127, 101, 127, 101, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 102 Glock -> Glockenspiel [semantic] */
    { 127, 102, 127, 102, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 103 Tube Bell -> Tubular-Bell [semantic] */
    { 127, 103, 127, 103, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 104 Xylophone -> Xylophone [exact] */
    { 127, 104, 127, 104, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 105 Marimba -> Marimba [exact] */
    { 127, 105, 127, 105, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 106 Koto -> Koto [exact] */
    { 127, 106, 127, 106, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 107 Sho -> Taisho Koto [semantic] */
    { 127, 107, 127, 107, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 108 Shakuhachi -> Shakuhachi [exact] */
    { 127, 108, 127, 108, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 109 Whistle 1 -> Whistle [semantic] */
    { 127, 109, 127, 108, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 110 Whistle 2 -> Whistle [semantic] */
    { 127, 110, 127, 110, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 111 Bottleblow -> Bottle Blow [semantic] */
    { 127, 111, 127, 111, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 112 Breathpipe -> Pan Flute [semantic] */
    { 127, 112, 127, 112, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 113 Timpani -> Timpani [exact] */
    { 127, 113, 127, 113, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 114 Melodic Tom -> Melo Tom [semantic] */
    { 127, 114, 127, 115, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 115 Deep Snare -> Synth Drum [semantic] */
    { 127, 115, 127, 115, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 116 Elec Perc 1 -> Synth Drum [semantic] */
    { 127, 116, 127, 116, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 117 Elec Perc 2 -> Synth Drum [semantic] */
    { 127, 117, 127, 117, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 118 Taiko -> Taiko [exact] */
    { 127, 118, 127, 118, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 119 Taiko Rim -> Taiko [semantic] */
    { 127, 119, 127, 119, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 120 Cymbal -> Reverse Cym. [semantic] */
    { 127, 120, 127, 120, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 121 Castanets -> Castanets [exact] */
    { 127, 121, 127, 121, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 122 Triangle -> Tinkle Bell [fallback] */
    { 127, 122, 127, 122, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 123 Orche Hit -> Orchestra Hit [semantic] */
    { 127, 123, 127, 123, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 124 Telephone -> Telephone [exact] */
    { 127, 124, 127, 124, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 125 Bird Tweet -> Bird [semantic] */
    { 127, 125, 127, 122, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 126 One Note Jam -> Orchestra Hit [fallback] */
    { 127, 126, 127, 100, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 127 Water Bell -> Tinkle Bell [semantic] */
    { 127, 127, 127, 127, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* MT32 128 Jungle Tune -> Ice Rain [fallback] */
    {   8,   0,   0,   0, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Piano 1w -> GM capital same PC */
    {  16,   0,   0,   0, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Piano 1d -> GM capital same PC */
    {   8,   1,   0,   1, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Piano 2w -> GM capital same PC */
    {   8,   2,   0,   2, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Piano 3w -> GM capital same PC */
    {   8,   3,   0,   3, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Honky-tonk w -> GM capital same PC */
    {   8,   4, 127,   3, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned EP 1 -> Detuned EP 1 */
    {  16,   4,   0,   4, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* E. Piano 1v -> GM capital same PC */
    {  24,   4,   0,   4, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* 60's E. Piano -> GM capital same PC */
    {   8,   5, 127,   6, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned EP 2 -> Detuned EP 2 */
    {  16,   5,   0,   5, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* E. Piano 2v -> GM capital same PC */
    {   8,   6, 127,  17, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Coupled Hps. -> Coupled Hps. */
    {  16,   6,   0,   6, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Harpsi.w -> GM capital same PC */
    {  24,   6,   0,   6, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Harpsi.o -> GM capital same PC */
    {   8,  11,   0,  11, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Vib.w -> GM capital same PC */
    {   8,  12,   0,  12, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Marimba w -> GM capital same PC */
    {   8,  14,   0,  14, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Church Bell -> GM capital same PC */
    {   9,  14,   0,  14, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Carillon -> GM capital same PC */
    {   8,  16, 127,  11, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned Or. 1 -> Detuned Or. 1 */
    {  16,  16,   0,  16, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* 60's Organ 1 -> GM capital same PC */
    {  32,  16,   0,  16, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Organ 4 -> GM capital same PC */
    {   8,  17,   0,  17, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned Or. 2 -> GM capital same PC */
    {  32,  17,   0,  17, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Organ 5 -> GM capital same PC */
    {   8,  19, 127,  12, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Church Org.2 -> Church Org. 2 */
    {  16,  19,   0,  19, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Church Org.3 -> GM capital same PC */
    {   8,  21,   0,  21, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Accordion It -> GM capital same PC */
    {   8,  24,   0,  24, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Ukulele -> GM capital same PC */
    {  16,  24,   0,  24, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Nylon Gt.o -> GM capital same PC */
    {  32,  24,   0,  24, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Nylon Gt.2 -> GM capital same PC */
    {   8,  25,   0,  25, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* 12-str. Gt. -> GM capital same PC */
    {  16,  25,   0,  25, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Mandolin -> GM capital same PC */
    {   8,  26,   0,  26, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Hawaiian Gt. -> GM capital same PC */
    {   8,  27, 127,  61, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Chorus Gt. -> Chorus Gt. */
    {   8,  28, 127,  62, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Funk Gt. -> Funk Gt. */
    {  16,  28,   0,  28, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Funk Gt.2 -> GM capital same PC */
    {   8,  30,   0,  30, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Feedback Gt. -> GM capital same PC */
    {   8,  31,   0,  31, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Gt. Feedback -> GM capital same PC */
    {   1,  38, 127,  28, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Bass 101 -> MT-32 Synth Bass1 */
    {   8,  38, 127,  30, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Bass 3 -> Synth Bass 3 */
    {   8,  39, 127,  31, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Bass 4 -> Synth Bass 4 */
    {  16,  39, 127,  29, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Rubber Bass -> MT-32 Synth Bass2 */
    {   8,  40,   0,  40, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Slow Violin -> GM capital same PC */
    {   8,  48,   0,  48, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Orchestra -> GM capital same PC */
    {   8,  50,   0,  50, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Syn. Strings3 -> GM capital same PC */
    {  32,  52, 127,  34, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Choir Aahs 2 -> MT-32 Choir Aahs */
    {   1,  57, 127,  91, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Trombone 2 -> MT-32 Trombone #2 */
    {   1,  60, 127,  92, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Fr. Horn -> French Horn */
    {   8,  61, 127,  96, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Brass 2 -> Brass 2 */
    {   8,  62, 127,  26, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Brass3 -> Synth Brass3 */
    {  16,  62, 127,  24, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* AnalogBrass1 -> MT-32 Synth Brass1 */
    {   8,  63, 127,  27, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Brass4 -> Synth Brass4 */
    {  16,  63, 127,  25, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* AnalogBrass2 -> MT-32 Synth Brass2 */
    {   1,  80, 127,  47, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Square -> Square Wave */
    {   8,  80,   0,  80, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Sine Wave -> GM capital same PC */
    {   1,  81, 127,  44, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Saw -> Saw Wave */
    {   8,  81, 127,  44, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Doctor Solo -> MT-32 Saw Wave */
    {   8, 107, 127, 106, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Taisho Koto -> Taisho Koto */
    {   8, 115, 127, 120, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Castanets -> Castanets */
    {   8, 116,   0, 116, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Concert BD -> GM capital same PC */
    {   8, 117, 127, 114, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Melo Tom 2 -> MT-32 Melo Tom */
    {   8, 118,   0, 118, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* 808 Tom -> GM capital same PC */
    {   1, 120,   0, 120, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Gt. Cut Noise -> GM capital same PC */
    {   2, 120,   0, 120, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* String Slap -> GM capital same PC */
    {   1, 121,   0, 121, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Fl. Key Click -> GM capital same PC */
    {   1, 122,   0,  96, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Rain -> FX 1 (rain) */
    {   2, 122,   0, 122, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Thunder -> GM capital same PC */
    {   3, 122,   0, 122, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Wind -> GM capital same PC */
    {   4, 122,   0, 122, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Stream -> GM capital same PC */
    {   5, 122,   0, 122, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Bubble -> GM capital same PC */
    {   1, 123,   0, 123, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Dog -> GM capital same PC */
    {   2, 123,   0, 123, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Horse-Gallop -> GM capital same PC */
    {   1, 124,   0, 124, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Telephone 2 -> GM capital same PC */
    {   2, 124,   0, 124, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Door Creaking -> GM capital same PC */
    {   3, 124,   0, 124, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Door -> GM capital same PC */
    {   4, 124,   0, 124, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Scratch -> GM capital same PC */
    {   5, 124,   0, 124, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Windchime -> GM capital same PC */
    {   1, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Car-Engine -> GM capital same PC */
    {   2, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Car-Stop -> GM capital same PC */
    {   3, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Car-Pass -> GM capital same PC */
    {   4, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Car-Crash -> GM capital same PC */
    {   5, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Siren -> GM capital same PC */
    {   6, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Train -> GM capital same PC */
    {   7, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Jetplane -> GM capital same PC */
    {   8, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Starship -> GM capital same PC */
    {   9, 125,   0, 125, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Burst Noise -> GM capital same PC */
    {   1, 126,   0, 126, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Laughing -> GM capital same PC */
    {   2, 126,   0, 126, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Screaming -> GM capital same PC */
    {   3, 126,   0, 126, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Punch -> GM capital same PC */
    {   4, 126,   0, 126, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Heart Beat -> GM capital same PC */
    {   5, 126,   0, 126, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Footsteps -> GM capital same PC */
    {   1, 127,   0, 127, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Machine Gun -> GM capital same PC */
    {   2, 127,   0, 127, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Lasergun -> GM capital same PC */
    {   3, 127,   0, 127, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Explosion -> GM capital same PC */
};

/* V5 family fallbacks are consulted only when no exact variation rule exists
 * and the source bank is neither target-native GM(0) nor MT-32(127). They
 * provide a deterministic same-family Bank-127 target before the generic
 * GM-capital collapse. */
static const gs2sam_tone_family_rule_t k_sc55_family_rules[] = {
    {  19, 127,  13 }, /* Church Organ -> Church Org. */
    {  38, 127,  28 }, /* Synth Bass 1 -> Synth Bass1 */
    {  39, 127,  29 }, /* Synth Bass 2 -> Synth Bass2 */
    {  52, 127,  34 }, /* Choir Aahs -> Choir Aahs */
    {  57, 127,  91 }, /* Trombone -> Trombone #2 */
    {  60, 127,  92 }, /* French Horn -> French Horn */
    {  61, 127,  96 }, /* Brass Section -> Brass 2 */
    {  62, 127,  24 }, /* Synth Brass 1 -> Synth Brass1 */
    {  63, 127,  25 }, /* Synth Brass 2 -> Synth Brass2 */
    {  80, 127,  47 }, /* Lead 1 family -> Square Wave */
    {  81, 127,  44 }, /* Lead 2 family -> Saw Wave */
    { 117, 127, 114 }, /* Melodic Tom family -> Melo Tom */
};

static const gs2sam_drum_kit_rule_t k_sc55_drum_kits[] = {
    {   0,   0, 0 }, /* STANDARD -> STANDARD */
    {   8,   0, 0 }, /* ROOM -> STANDARD */
    {  16,  16, 0 }, /* POWER -> POWER */
    {  24,  16, 0 }, /* ELECTRONIC -> POWER */
    {  25,   0, 0 }, /* TR-808 -> STANDARD */
    {  32,   0, 0 }, /* JAZZ -> STANDARD */
    {  40,  40, 0 }, /* BRUSH -> BRUSH */
    {  48,  48, 0 }, /* ORCHESTRA -> ORCHESTRA */
    {  56, 127, GS2SAM_DRUM_KIT_DROP_UNMAPPED }, /* SFX -> CM-64/32 partial */
    { 127, 127, 0 }, /* CM-64/32 -> CM-64/32 partial */
};

static const gs2sam_drum_note_rule_t k_sc55_drum_notes[] = {
    {  24,  38,  40 }, /* Elec SD -> Snare Drum 2 */
    {  24,  40,  38 }, /* Gated SD -> Gated Snare */
    {  56,  58,  82 }, /* Applause -> Applauses */
    {  56,  70,  94 }, /* Helicopter -> Helicopter */
    {  56,  72,  96 }, /* Gun Shot -> Gun Shot */
    {  56,  78, 102 }, /* Birds -> Birds */
    {  56,  82, 106 }, /* Seashore -> SeaShore */
};

const gs2sam_tone_rule_t *gs2sam_sc55mk2_tone_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55_rules) / sizeof(k_sc55_rules[0]);
    return k_sc55_rules;
}

const gs2sam_tone_family_rule_t *gs2sam_sc55mk2_tone_family_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55_family_rules) / sizeof(k_sc55_family_rules[0]);
    return k_sc55_family_rules;
}

const gs2sam_drum_kit_rule_t *gs2sam_sc55mk2_drum_kit_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55_drum_kits) / sizeof(k_sc55_drum_kits[0]);
    return k_sc55_drum_kits;
}

const gs2sam_drum_note_rule_t *gs2sam_sc55mk2_drum_note_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55_drum_notes) / sizeof(k_sc55_drum_notes[0]);
    return k_sc55_drum_notes;
}

void gs2sam_sc55mk2_config(gs2sam_config_t *cfg,
                           gs2sam_emit_fn emit,
                           void *emit_user)
{
    size_t n = 0;
    gs2sam_default_config(cfg, emit, emit_user);
    cfg->tone_rules = gs2sam_sc55mk2_tone_rules(&n);
    cfg->tone_rule_count = n;
    cfg->tone_family_rules = gs2sam_sc55mk2_tone_family_rules(&n);
    cfg->tone_family_rule_count = n;
    cfg->drum_kit_rules = gs2sam_sc55mk2_drum_kit_rules(&n);
    cfg->drum_kit_rule_count = n;
    cfg->drum_note_rules = gs2sam_sc55mk2_drum_note_rules(&n);
    cfg->drum_note_rule_count = n;
    cfg->rhythm_volume_percent = 100u;
}
