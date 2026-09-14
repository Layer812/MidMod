#include "gs2sam_profile_sc55mk2.h"

/* MIDI program numbers below are zero-based. Source names are from the
 * SC-55mkII Instrument Table; target names are from the SAM2695 MT-32 Sound
 * Variation #127 table. Only clear name/role matches are included. */
static const gs2sam_tone_rule_t k_sc55mk2_rules[] = {
    /* CC0  src PC             -> CC0 dst PC */
    {  8,   4,                 127,  3, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned EP 1 -> Detuned EP1 */
    {  8,   5,                 127,  6, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned EP 2 -> Detuned EP2 */
    {  8,   6,                 127, 17, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Coupled Hps. -> Coupled Hps. */
    {  8,  16,                 127, 11, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Detuned Or.1 -> Detuned Or.1 */
    {  8,  19,                 127, 12, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Church Org.2 -> Church Org.2 */
    {  8,  27,                 127, 61, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Chorus Gt. -> Chorus Gt. */
    {  8,  28,                 127, 62, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Funk Gt. -> Funk Gt. */
    {  8,  38,                 127, 30, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Bass 3 -> Synth Bass3 */
    {  8,  39,                 127, 31, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Bass 4 -> Synth Bass4 */
    {  1,  60,                 127, 92, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Fr. Horn -> French Horn */
    {  8,  61,                 127, 96, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Brass 2 -> Brass 2 */
    {  8,  62,                 127, 26, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Brass3 -> Synth Brass3 */
    {  8,  63,                 127, 27, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Synth Brass4 -> Synth Brass4 */
    {  1,  80,                 127, 47, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Square -> Square Wave */
    {  1,  81,                 127, 44, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Saw -> Saw Wave */
    {  8, 107,                 127,106, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Taisho Koto -> Taisho Koto */
    {  8, 115,                 127,120, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }, /* Castanets -> Castanets */
    {  1, 122,                   0, 96, 0, 0,0,0,0,0,0,0,0, GS2SAM_BANK_LSB_ANY }  /* Rain -> GM FX 1 (rain) */
};

/* SC-55mkII drum programs (source) retargeted to the closest SAM2695 native
 * set. R3 deliberately avoids invented timbre controls: kit selection and
 * sparse note remapping only. */
static const gs2sam_drum_kit_rule_t k_sc55mk2_drum_kits[] = {
    {  0,   0,   0 }, /* PC 1   STANDARD   -> STANDARD */
    {  8,   0,   0 }, /* PC 9   ROOM       -> STANDARD (same note semantics) */
    { 16,  16,   0 }, /* PC 17  POWER      -> POWER */
    { 24,  16,   0 }, /* PC 25  ELECTRONIC -> POWER + sparse snare remap */
    { 25,   0,   0 }, /* PC 26  TR-808     -> STANDARD (timbre unavailable) */
    { 32,   0,   0 }, /* PC 33  JAZZ       -> STANDARD */
    { 40,  40,   0 }, /* PC 41  BRUSH      -> BRUSH */
    { 48,  48,   0 }, /* PC 49  ORCHESTRA  -> ORCHESTRA */
    { 56, 127, GS2SAM_DRUM_KIT_DROP_UNMAPPED }, /* PC 57 SFX -> CM-64/32 */
    {127, 127,   0 }  /* PC 128 CM-64/32   -> CM-64/32 */
};

/* Sparse semantic note remaps.
 * ELECTRONIC: the SAM POWER kit has Gated Snare at note 38; swap the two
 * source snare roles so source Gated SD (40) reaches it, while source Elec SD
 * (38) gets the ordinary Snare Drum 2 at target note 40.
 *
 * SFX: only same-name/role sounds actually documented in the SAM2695 CM set
 * are emitted. All other PC57 notes are suppressed by DROP_UNMAPPED. */
static const gs2sam_drum_note_rule_t k_sc55mk2_drum_notes[] = {
    {24, 38, 40}, /* ELECTRONIC Elec SD -> Snare Drum 2 (approx.) */
    {24, 40, 38}, /* ELECTRONIC Gated SD -> POWER Gated Snare */

    {56, 58, 82},  /* SFX Applause   -> CM Applauses */
    {56, 70, 94},  /* SFX Helicopter -> CM Helicopter */
    {56, 72, 96},  /* SFX Gun Shot   -> CM Gun Shot */
    {56, 78,102},  /* SFX Birds      -> CM Birds */
    {56, 82,106}   /* SFX Seashore   -> CM SeaShore */
};

const gs2sam_tone_rule_t *gs2sam_sc55mk2_tone_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55mk2_rules) / sizeof(k_sc55mk2_rules[0]);
    return k_sc55mk2_rules;
}

const gs2sam_drum_kit_rule_t *gs2sam_sc55mk2_drum_kit_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55mk2_drum_kits) / sizeof(k_sc55mk2_drum_kits[0]);
    return k_sc55mk2_drum_kits;
}

const gs2sam_drum_note_rule_t *gs2sam_sc55mk2_drum_note_rules(size_t *count)
{
    if (count) *count = sizeof(k_sc55mk2_drum_notes) / sizeof(k_sc55mk2_drum_notes[0]);
    return k_sc55mk2_drum_notes;
}

void gs2sam_sc55mk2_config(gs2sam_config_t *cfg,
                           gs2sam_emit_fn emit,
                           void *emit_user)
{
    size_t n = 0;
    gs2sam_default_config(cfg, emit, emit_user);
    cfg->tone_rules = gs2sam_sc55mk2_tone_rules(&n);
    cfg->tone_rule_count = n;
    cfg->drum_kit_rules = gs2sam_sc55mk2_drum_kit_rules(&n);
    cfg->drum_kit_rule_count = n;
    cfg->drum_note_rules = gs2sam_sc55mk2_drum_note_rules(&n);
    cfg->drum_note_rule_count = n;
}
