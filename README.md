# lba-midi-play

A standalone C99 command-line tool for playing the MIDI music from
**Little Big Adventure 1** (Relentless: Twinsen's Adventure, 1994) directly
from the original `MIDI_MI.HQR` and `MIDI_SB.HQR` game files.

Uses only single-header C libraries: **TinySoundFont** (TSF + TML) for
synthesis and **miniaudio** for audio output. No external library dependencies
beyond what the OS provides.

---

## Build

```sh
# Clone or copy this directory alongside your game files
make

# macOS links CoreAudio/AudioToolbox automatically.
# Linux needs: -lpthread -ldl -lm  (the Makefile handles this).
```

Requirements: a C99 compiler (`cc`), `make`, and `tsf.h` / `tml.h` /
`miniaudio.h` already present (downloaded by the repo).  A soundfont is
loaded at runtime — see [Soundfonts](#soundfonts) below.

---

## Usage

```sh
# List tracks in an HQR file
./lba-midi-play MIDI_MI.HQR

# Play a track (uses the first system soundfont it finds)
./lba-midi-play MIDI_MI.HQR 3

# Play with a specific soundfont
./lba-midi-play MIDI_MI.HQR 3 /path/to/soundfont.sf2

# Play the Sound Blaster version of the same track
./lba-midi-play MIDI_SB.HQR 3
```

Press **Enter** to stop playback.

---

## The Two MIDI Files

LBA1 ships with two MIDI archives, each containing 33 tracks (indices 0–32):

| File | Audience | Target hardware |
|------|----------|-----------------|
| `MIDI_MI.HQR` | MIDI Interface | Roland MT-32 / Sound Canvas SC-55 or any GM MIDI port |
| `MIDI_SB.HQR` | Sound Blaster | Sound Blaster OPL/FM or AWE32 wavetable |

Both files are XMIDI IFF containers compressed with LBA's LZ variant inside
an HQR archive.

### MIDI_MI — the "MIDI Interface" version

Intended for an external MIDI port driving a Roland Sound Canvas SC-55 (the
canonical LBA instrument — what the composers heard).  Arrangements are
**richer**: more simultaneous voices, instruments such as Fretless Bass,
Orchestral Harp, Synth Brass, Piccolo, and layered pad sounds.  File sizes
are typically **larger** (e.g. track 8: 21 980 bytes vs 10 540 in MIDI_SB).

### MIDI_SB — the "Sound Blaster" version

Intended for a Sound Blaster card (OPL2/OPL3 FM chip, or later the AWE32
wavetable).  Arrangements are **simplified**: fewer simultaneous voices,
simpler instrument choices.  The OPL FM chips have limited polyphony and
cannot reproduce complex pads or layered strings, so tracks were re-arranged
accordingly.

### Which sounds better through a modern software synthesiser?

With a high-quality General MIDI soundfont (FluidR3, GeneralUser GS,
MuseScore, etc.) **MIDI_MI** is usually more faithful to the composers'
intent — it has more notes, more instruments, and richer harmonics.  MIDI_SB
may occasionally win on specific tracks if the simplified arrangement happens
to sit better with a particular soundfont.  Try both.

---

## Track List

All 33 tracks are present in both files at the same index.  Track assignment
is embedded in the SCENE.HQR data (first byte of each scene header).

| Index | Duration | Used in game | Notes |
|-------|----------|--------------|-------|
| 0 | tiny (242 B) | Scenes without music | Single Recorder note — effectively silence/placeholder |
| 1 | long | Main menu; Citadel Island (scenes 10–13, 17–32, etc.) | The most-heard theme |
| 2 | long | Citadel Island caves / exterior variations (scenes 8, 36–41, 57) | |
| 3 | long | Principal Island / desert areas (scenes 42–55) | |
| 4 | medium | Special event / transition scenes (59, 60) | |
| 5 | long | Hamalayi / Tippett Island area (scenes 9, 62–72, 82) | |
| 6 | medium | Northern regions (scenes 15, 67, 68, 73, 81, 91, 92, 96) | |
| 7 | long | Zeelich / late-game areas (scenes 74–80, 101) | |
| 8 | very long | Island of the White Cliff / end-game (scenes 94–99, 103) | Identical to tracks 9 and 32 |
| 9 | very long | Same music as 8 — played in a different set of scenes (83–93, 100) | Byte-for-byte copy of track 8 |
| 10 | short | Short scene/cutscene sting | |
| 11–17 | short | Various cutscene / ambient stings | |
| 18 | tiny | Same as track 0 | |
| 19–23 | short | Cutscene / event stings | |
| 24–25 | medium | Identical tracks — ambient loop | |
| 26 | tiny | Single Recorder sting | |
| 27–30 | short–medium | Various scene stings | |
| 31 | tiny (166 B) | **Adeline Software logo screen** | Vibraphone — the Adeline jingle |
| 32 | very long | Same music as tracks 8 and 9 | Byte-for-byte copy |

> **Note on duplicates:** Tracks 8, 9, and 32 are byte-for-byte identical
> in MIDI_MI (all 21 980 bytes decompressed) and also identical in MIDI_SB
> (10 540 bytes).  The game plays them for different scene ranges, likely
> because the script engine triggers a track by index and some scenes
> redundantly reference the same music.

---

## HQR File Format

HQR (High Quality Resource) is Adeline's general-purpose asset container.

```
File layout
───────────
Offset 0x00   uint32  Offset to entry 0's data  (= num_entries * 4; doubles
                       as the index table size, used to compute entry count)
Offset 0x04   uint32  Offset to entry 1's data
…
Offset N*4    uint32  Offset to entry N's data
Offset (N+1)*4  ← sentinel — equals the file size

At each data offset
───────────────────
+0  uint32  Decompressed (real) size
+4  uint32  Compressed size
+8  uint16  Compression mode
+10 bytes   Compressed (or raw) data

Compression modes
─────────────────
0   Uncompressed — data is read directly
1   LBA LZ mode 1 — back-reference length = (offset & 0x0F) + 2
2   LBA LZ mode 2 — back-reference length = (offset & 0x0F) + 3
```

All MIDI entries in the game's MIDI HQR files use compression mode 1.
The compression ratio is roughly 2:1 (e.g. track 3: 8 612 bytes real,
4 790 bytes stored).

---

## XMIDI Format

LBA uses the **XMIDI** format produced by Miles Sound System (MSS), Intellicom's
audio library.  XMIDI is an IFF FORM container wrapping MIDI event data.

```
IFF structure
─────────────
FORM <length>
  XDIR
    INFO <2>    num_tracks (little-endian uint16)
CAT  <length>
  XMID
    FORM <length>
      XMID
        TIMB  (optional: custom timbres — ignored)
        EVNT <length>  ← MIDI event stream starts here
```

Key differences from Standard MIDI File (SMF):

| Property | XMIDI | SMF |
|----------|-------|-----|
| Delta time encoding | Custom VLQ: sum non-high-bit bytes (`readVLQ2`) | Standard VLQ |
| Note duration | Stored inline after Note-On velocity | Explicit Note-Off event |
| Tempo | Forced to 500 000 µs/beat during conversion | As written |
| Controllers 0x6E–0x78 | Miles-specific (loop, branch, timbres) | N/A |

The converter (`xmidi.c`, adapted from ScummVM/Exult via TwinEngine) produces
**SMF format 0** at 60 PPQN, tempo 500 000 µs/beat (= 120 BPM).

---

## Soundfonts

The tool discovers soundfonts in this order:

1. Path given as third argument: `./lba-midi-play MIDI_MI.HQR 3 my.sf2`
2. `/Library/Audio/Sounds/Banks/FluidR3_GM_GS.sf2` (macOS)
3. `/Library/Audio/Sounds/Banks/Roland_SC-55_v3.7.sf2` (macOS)
4. `/usr/share/sounds/sf2/FluidR3_GM.sf2` (Linux)
5. `/usr/share/soundfonts/FluidR3_GM.sf2` (Linux)
6. `/usr/share/sounds/sf2/TimGM6mb.sf2` (Linux)

### Recommended soundfonts

| Soundfont | Size | Notes |
|-----------|------|-------|
| **FluidR3_GM_GS.sf2** | 144 MB | Excellent all-rounder; macOS system install |
| **Roland SC-55 v3.7.sf2** | 103 MB | Closest to the original composers' monitor; macOS system install |
| **GeneralUser GS** | ~30 MB | Lightweight, good GM coverage. Download from https://schristiancollins.com/generaluser.php |
| **MuseScore_General.sf3** | ~43 MB | Modern compressed SF3 (not supported by TSF — TSF needs SF2) |
| **TimGM6mb.sf2** | 6 MB | Tiny, usable, common on Linux |

### The Roland SC-55 note

LBA's music was composed and arranged specifically for the **Roland Sound
Canvas SC-55** — the gold standard consumer GM synth of 1994.  If you have
access to a Roland SC-55 sample library in SF2 format, that is the most
historically accurate playback.  The macOS system install of
`Roland_SC-55_v3.7.sf2` is a good approximation.

To use it:
```sh
./lba-midi-play MIDI_MI.HQR 3 \
  "/Library/Audio/Sounds/Banks/Roland_SC-55_v3.7.sf2"
```

---

## Implementation Notes

### Architecture

```
main.c       CLI, argument parsing, miniaudio setup, playback loop
hqr.c/h      HQR decompression — adapted from TwinEngine (twin-e/src/hqr.c)
xmidi.c/h    XMIDI → SMF conversion — adapted from TwinEngine via ScummVM/Exult
tsf.h        TinySoundFont — SF2 synthesiser (single-header, Bernhard Schelling)
tml.h        TML — SMF parser (single-header, Bernhard Schelling)
miniaudio.h  Cross-platform audio I/O (single-header, David Reid / mackron)
```

### Audio pipeline

```
MIDI_MI.HQR (on disk)
      │  hqr_get_entry_alloc()  — LZ decompress
      ▼
XMIDI bytes (in memory)
      │  convert_to_midi()      — XMIDI IFF → SMF format 0
      ▼
SMF bytes (in memory)
      │  tml_load_memory()      — parse events, build linked list
      ▼
tml_message list
      │  audio callback: advance time, fire tsf_channel_* per event
      ▼
TSF rendered frames (float32, 44 100 Hz stereo)
      │  miniaudio data callback
      ▼
Speaker output
```

### Timing

The audio callback advances playback time by `1000 / 44100 ≈ 0.023 ms` per
sample.  MIDI events are fired when `current_time_ms >= event->time`.  This
gives sample-accurate timing at the cost of slightly higher per-frame CPU (one
MIDI check per sample), which is negligible for MIDI densities typical of
these tracks.

### Credits

- **LBA MIDI music** — Adeline Software International / Philippe Vachey (1994)
- **TwinEngine** (HQR + XMIDI code) — The TwinEngine team (GPL v2)
- **XMIDI conversion** — Originally ScummVM / Exult projects (GPL v2)
- **TinySoundFont / TML** — Bernhard Schelling (MIT)
- **miniaudio** — David Reid (MIT / public domain)
