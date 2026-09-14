/* Architecture example only; adapt queue names to the PX68 host. */
#include <midi_engine.h>
#include <gs2sam_processor.h>

static midi_engine_t g_midi;
static gs2sam_processor_t g_gs2sam;

static void px68_midi_backend_write(void *user, const uint8_t *p, size_t n)
{
    (void)user;
    (void)p;
    (void)n;
    /* Enqueue to the existing CPU0/host MIDI worker here.
       Never make guest CPU execution wait on the physical UART. */
    /* retrop4_midi_queue_write(p, n); */
}

static void px68_midi_backend_volume(void *user, uint8_t volume)
{
    (void)user;
    (void)volume;
    /* retrop4_midi_queue_set_volume(volume); */
}

void px68_midi_engine_init(void)
{
    const midi_engine_backend_t be = {
        px68_midi_backend_write,
        px68_midi_backend_volume,
        0
    };
    midi_engine_init(&g_midi, &be, 32u);

    /* Optional. Omit these four lines for byte-exact pass-through to a
       modern/fully compatible target module. */
    gs2sam_processor_init_sc55mk2(&g_gs2sam, &g_midi, 88u);
    midi_processor_t proc = gs2sam_processor_interface(&g_gs2sam);
    midi_engine_set_processor(&g_midi, &proc);
    midi_engine_set_profile_label(&g_midi, "SC-55mkII / GS");
}

void px68_ym3802_tx_byte(uint8_t value)
{
    midi_engine_feed_byte(&g_midi, value);
}
