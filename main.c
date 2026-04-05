/*
 * lba-midi-play — play MIDI tracks from LBA1's midi_mi.hqr
 *
 * Usage:
 *   ./lba-midi-play <hqr>                     -- list track count
 *   ./lba-midi-play <hqr> <index>             -- play with default soundfont
 *   ./lba-midi-play <hqr> <index> <sf2>       -- play with given soundfont
 *
 * Press Enter to stop playback.
 *
 * Build dependencies (single-header, included here):
 *   tsf.h       — TinySoundFont synthesiser
 *   tml.h       — TML MIDI parser
 *   miniaudio.h — cross-platform audio I/O
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ---------- single-header library implementations ---------- */
#define TSF_IMPLEMENTATION
#include "tsf.h"

#define TML_IMPLEMENTATION
#include "tml.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

/* ---------- our modules ---------- */
#include "hqr.h"
#include "xmidi.h"

/* -------------------------------------------------------------------------
 * Default soundfont search paths (tried in order when no sf2 arg given)
 * ---------------------------------------------------------------------- */

static const char *DEFAULT_SF2[] = {
    /* macOS system soundfonts */
    "/Library/Audio/Sounds/Banks/FluidR3_GM_GS.sf2",
    "/Library/Audio/Sounds/Banks/Roland_SC-55_v3.7.sf2",
    /* Common Linux paths */
    "/usr/share/sounds/sf2/FluidR3_GM.sf2",
    "/usr/share/soundfonts/FluidR3_GM.sf2",
    "/usr/share/sounds/sf2/TimGM6mb.sf2",
    NULL
};

static const char *find_default_sf2(void)
{
    for (int i = 0; DEFAULT_SF2[i]; i++) {
        FILE *f = fopen(DEFAULT_SF2[i], "rb");
        if (f) { fclose(f); return DEFAULT_SF2[i]; }
    }
    return NULL;
}

/* -------------------------------------------------------------------------
 * Playback state shared between main thread and audio callback
 * ---------------------------------------------------------------------- */

#define SAMPLE_RATE 44100
#define CHANNELS    2

typedef struct {
    tsf         *sf;
    tml_message *midi;     /* current position in the TML message list */
    double       time_ms;  /* playback time in milliseconds */
    int          done;     /* set to 1 when all events have been processed */
} PlayState;

/* -------------------------------------------------------------------------
 * miniaudio data callback — called from the audio thread
 * ---------------------------------------------------------------------- */

static void dispatch_event(PlayState *ps, tml_message *m)
{
    switch (m->type) {
    case TML_PROGRAM_CHANGE:
        tsf_channel_set_presetnumber(ps->sf, m->channel, m->program,
            (m->channel == 9));
        break;
    case TML_NOTE_ON:
        if (m->velocity > 0)
            tsf_channel_note_on(ps->sf, m->channel, m->key,
                m->velocity / 127.0f);
        else
            tsf_channel_note_off(ps->sf, m->channel, m->key);
        break;
    case TML_NOTE_OFF:
        tsf_channel_note_off(ps->sf, m->channel, m->key);
        break;
    case TML_PITCH_BEND:
        tsf_channel_set_pitchwheel(ps->sf, m->channel, m->pitch_bend);
        break;
    case TML_CONTROL_CHANGE:
        tsf_channel_midi_control(ps->sf, m->channel, m->control,
            m->control_value);
        break;
    default:
        break;
    }
}

static void audio_callback(ma_device *device, void *output,
                            const void *input, ma_uint32 frame_count)
{
    PlayState *ps        = (PlayState *)device->pUserData;
    float     *out       = (float *)output;
    ma_uint32  remaining = frame_count;

    (void)input;

    if (ps->done) {
        memset(output, 0, frame_count * CHANNELS * sizeof(float));
        return;
    }

    while (remaining > 0) {
        /*
         * Render a batch of frames up to — but not including — the next
         * MIDI event.  This replaces the previous per-sample loop and
         * reduces tsf_render_float call count from ~44 100/s to ~events/s.
         */
        ma_uint32 block = remaining;
        if (ps->midi) {
            double samps = (ps->midi->time - ps->time_ms) * SAMPLE_RATE / 1000.0;
            if (samps <= 0.0) {
                block = 0; /* event overdue: fire before rendering */
            } else {
                ma_uint32 s = (ma_uint32)samps;
                /* If samps rounds down to 0 (sub-sample gap), render 1 frame
                 * so time advances past the event on the next dispatch. */
                if (s == 0) s = 1;
                if (s < block) block = s;
            }
        }

        if (block > 0) {
            tsf_render_float(ps->sf, out, (int)block, 0);

            /*
             * Soft limiter: tanhf saturates smoothly to ±1.0 instead of
             * hard-clipping when multiple voices sum past the float ceiling.
             * tanhf(x) ≈ x for |x| << 1 so quiet passages are unaffected.
             */
            for (ma_uint32 i = 0; i < block * CHANNELS; i++)
                out[i] = tanhf(out[i]);

            out            += block * CHANNELS;
            ps->time_ms    += (double)block * 1000.0 / SAMPLE_RATE;
            remaining      -= block;
        }

        /* Dispatch all events whose time has arrived */
        while (ps->midi && ps->time_ms >= ps->midi->time) {
            dispatch_event(ps, ps->midi);
            ps->midi = ps->midi->next;
            if (!ps->midi) { ps->done = 1; break; }
        }

        if (ps->done) {
            memset(out, 0, remaining * CHANNELS * sizeof(float));
            break;
        }
    }
}

/* -------------------------------------------------------------------------
 * Main
 * ---------------------------------------------------------------------- */

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s <midi_mi.hqr> [track_index] [soundfont.sf2]\n",
            argv[0]);
        return 1;
    }

    const char *hqr_path = argv[1];

    /* --- List mode --- */
    if (argc == 2) {
        int n = hqr_get_num_entries(hqr_path);
        if (n < 0) {
            fprintf(stderr, "Error: could not read '%s'\n", hqr_path);
            return 1;
        }
        printf("%s: %d track(s) (indices 0..%d)\n", hqr_path, n, n - 1);
        return 0;
    }

    /* --- Play mode --- */
    int track_index = atoi(argv[2]);

    const char *sf2_path = (argc >= 4) ? argv[3] : find_default_sf2();
    if (!sf2_path) {
        fprintf(stderr,
            "Error: no soundfont found. Pass one as the third argument:\n"
            "  %s %s %d /path/to/soundfont.sf2\n",
            argv[0], hqr_path, track_index);
        return 1;
    }
    printf("Soundfont : %s\n", sf2_path);

    /* 1. Load entry from HQR */
    uint8_t *raw_data = NULL;
    int raw_size = hqr_get_entry_alloc(&raw_data, hqr_path, track_index);
    if (!raw_size || !raw_data) {
        fprintf(stderr, "Error: failed to load track %d from '%s'\n",
                track_index, hqr_path);
        return 1;
    }

    /* 2. Convert XMIDI → SMF if needed, or use native SMF directly */
    uint8_t *smf_data = NULL;
    uint32_t smf_size = 0;

    if (raw_size >= 4 && memcmp(raw_data, "MThd", 4) == 0) {
        /* Already a standard MIDI file (e.g. Midi_mi_win.hqr) */
        printf("Loaded    : track %d (%d bytes, SMF native)\n",
               track_index, raw_size);
        smf_data = raw_data;
        smf_size = (uint32_t)raw_size;
        raw_data = NULL; /* ownership transferred to smf_data */
    } else {
        /* XMIDI IFF — convert to SMF */
        printf("Loaded    : track %d (%d bytes, XMIDI)\n",
               track_index, raw_size);
        smf_size = convert_to_midi(raw_data, (uint32_t)raw_size, &smf_data);
        free(raw_data);
        raw_data = NULL;
        if (!smf_size || !smf_data) {
            fprintf(stderr, "Error: XMIDI→SMF conversion failed for track %d\n",
                    track_index);
            return 1;
        }
        printf("Converted : %u bytes SMF\n", smf_size);
    }

    /* 3. Parse SMF with TML */
    tml_message *midi_head = tml_load_memory(smf_data, (int)smf_size);
    free(smf_data);
    if (!midi_head) {
        fprintf(stderr, "Error: TML failed to parse SMF data\n");
        return 1;
    }

    /* Count messages for info */
    int msg_count = 0;
    for (tml_message *m = midi_head; m; m = m->next) msg_count++;
    printf("Parsed    : %d MIDI messages\n", msg_count);

    /* 4. Load soundfont with TSF */
    tsf *sf = tsf_load_filename(sf2_path);
    if (!sf) {
        fprintf(stderr, "Error: failed to load soundfont '%s'\n", sf2_path);
        tml_free(midi_head);
        return 1;
    }
    tsf_set_output(sf, TSF_STEREO_INTERLEAVED, SAMPLE_RATE, 0.0f);
    /* GM channel 9 is percussion; it often has no Program Change event in the
     * MIDI data (the spec says it's always drums).  TSF defaults uninitialized
     * channels to preset 0 (piano), so we must prime it explicitly. */
    tsf_channel_set_presetnumber(sf, 9, 0, 1 /* isDrum */);
    printf("Presets   : %d\n", tsf_get_presetcount(sf));

    /* 5. Playback state */
    PlayState ps;
    memset(&ps, 0, sizeof(ps));
    ps.sf      = sf;
    ps.midi    = midi_head;
    ps.time_ms = 0.0;
    ps.done    = 0;

    /* 6. miniaudio device */
    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format   = ma_format_f32;
    cfg.playback.channels = CHANNELS;
    cfg.sampleRate        = SAMPLE_RATE;
    cfg.dataCallback      = audio_callback;
    cfg.pUserData         = &ps;

    ma_device device;
    if (ma_device_init(NULL, &cfg, &device) != MA_SUCCESS) {
        fprintf(stderr, "Error: failed to initialise audio device\n");
        tsf_close(sf);
        tml_free(midi_head);
        return 1;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        fprintf(stderr, "Error: failed to start audio device\n");
        ma_device_uninit(&device);
        tsf_close(sf);
        tml_free(midi_head);
        return 1;
    }

    printf("Playing   : track %d — press Enter to stop...\n", track_index);
    getchar();

    /* 7. Clean up */
    ma_device_stop(&device);
    ma_device_uninit(&device);
    tsf_close(sf);
    tml_free(midi_head);

    printf("Stopped.\n");
    return 0;
}
