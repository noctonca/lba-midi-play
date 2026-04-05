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

All 33 tracks (indices 0–32) are present in both MIDI_MI.HQR and MIDI_SB.HQR.

**How track assignment works:** each scene entry in SCENE.HQR carries its
music track index at **byte offset 38** of the decompressed header (verified
from `twin-e/src/scene.c` `loadScene()`).  Tracks not referenced from any
scene header are triggered by in-game script events (`PLAY_MIDI` opcode) or
hardcoded engine calls.  Scene names come from the ScummVM TwinE engine's
`LBA1SceneId` enum (`engines/twine/shared.h`).

| Idx | MI size | Trigger | Scene / context | Notes |
|-----|---------|---------|-----------------|-------|
| 0 | 242 B | Script only | — | Single Recorder note; identical to track 18. Never triggered from a scene header. |
| 1 | 7 274 B | Scene header + code | Principal Island Ruins (17), White Leaf Desert Maze (57), Brundle Island teleportation (95, 99); **main menu** (hardcoded in engine) | |
| 2 | 8 104 B | Scene header | Principal Island Harbor (11), Ticket Office (22), White Leaf Desert Temple 2nd (41), Hamalayi Mountains fighting scenes (62, 63, 69), Polar Island rocky peak (110, 111) | Action / battle theme |
| 3 | 8 612 B | Scene header | Rebellion Island Harbor (59), Rebellion Island Rebel camp (60) | |
| 4 | 1 768 B | Scene header | Citadel Island Warehouse (35), White Leaf Desert outside Temple of Bu (36), Fortress Island outside (84), Fortress Island Docks (100) | |
| 5 | 5 738 B | Scene header | Citadel Island near tavern (2) & pharmacy (3), White Leaf Desert Temple 1st (8) & 2nd (40), Hamalayi Mountains Prison (64), Fortress Island Secret passage (85), Principal Island inside fortress (105) | |
| 6 | 3 422 B | Scene header | Citadel Island Twinsen's house (5), Principal Island Library (10), Old Burg (13), inside Rabbibunny house (28), Stables (32), House with TV (58), Tippet Island Village (74) | |
| 7 | 9 394 B | Scene header | Hamalayi Mountains landing place (9), Mutation centre 1st (67), Catamaran dock (72) | |
| 8 | 21 980 B | Script only | — | Identical to tracks 9 and 32 (SHA256 confirmed). Never triggered from a scene header — played via script events. |
| 9 | 21 980 B | Scene header | Principal Island Port Belooga (24), Tippet Island near Dino-Fly (78), Hamalayi Mountains Ski resort (96), Principal Island house in Port Belooga (102) | Byte-for-byte copy of track 8 |
| 10 | 3 000 B | Scene header | Hamalayi Mountains Rabbibunny village (15), Principal Island outside library (18), Peg Leg Street (25), Bunker near clear water (73), Polar Island 2nd scene (106) | |
| 11 | 956 B | Script only | — | |
| 12 | 1 158 B | Scene header | Citadel Island Cellar of Tavern (33), Principal Island inside water tower (38), Hamalayi Mountains inside transporter (66), outside prison (71) | |
| 13 | 908 B | Scene header | Principal Island outside fortress (12), Military camp (19), White Leaf Desert Military camp (39), Proxima Island Proxim City (42), Hamalayi Entrance to prison (70), Tippet Island near bar (76) & Twinsun Café (80), Polar Island 4th scene (109) | Military / urban theme |
| 14 | 340 B | Scene header | Hamalayi Mountains Sacred Carrot (81), Polar Island Before the end room (112) | Sacred / final approach |
| 15 | 3 146 B | Scene header | Citadel Island Tavern (14), Architect's house (20), Principal Island Shop (26), Tavern (30), Proxima Island Shop (50), Inventor's house (54), Brundle Island Painter's house (103) | Civilian interiors — shops & taverns |
| 16 | 794 B | Scene header | Principal Island Prison (23), Astronomer's house (29), Proxima Island Prison (49), Fortress Island near Zoë's cell (87), Rune stone (90), Hamalayi Clear water lake (92), Brundle Island outside teleportation (94) | |
| 17 | 626 B | Scene header | Principal Island outside water tower (37), Proxima Island upper (45) & lower (46) rune stone, Fortress Island Cloning centre (89), Polar Island Final Battle (113) | |
| 18 | 242 B | Scene header | Citadel Island Prison (0), Pharmacy (7), secret chamber in house (21), Principal Island Locksmith (27), Basement of Astronomer (31), Proxima Island before upper rune stone (47) & Grobo house (53), cut-out room (61), Fortress Island inside (83), Brundle Island Secret room (98), Tippet Island Shop (101), Polar Island Before rocky peak (108) | **Silence / ambient placeholder** — the most-used "neutral" track (12 scenes). Identical to track 0. |
| 19 | 1 560 B | Scene header | Citadel Island outside citadel (1), near Twinsen's house (4), Tippet Island Secret passage 2 (75), Fortress Island Swimming pool (88), Citadel Island Ticket Office (104), Twinsen's house destroyed (118) | |
| 20 | 1 392 B | Scene header | Citadel Island Harbor (6), Hamalayi Mountains outside transporter (65), Tippet Island Secret passages 1 & 3 (77, 79), Brundle Island Docks (97), Polar Island 1st scene (115) | |
| 21 | 2 174 B | Scene header | Hamalayi Mountains Backdoor of the prison (82) | Single scene |
| 22 | 1 004 B | Scene header | Hamalayi Mountains Behind the Sacred Carrot (91) | Single scene |
| 23 | 1 734 B | Script only | — | |
| 24 | 3 052 B | Script only | — | Identical to track 25 (SHA256 confirmed) |
| 25 | 3 052 B | Script only | — | Identical to track 24 (SHA256 confirmed) |
| 26 | 322 B | Script only | — | Identical between MIDI_MI and MIDI_SB |
| 27 | 844 B | Scene header | Citadel Island inside Rabbibunny house (16), Sewer 1st scene (34), Proxima Island Forger's house (48), Sewer (51), Principal Island house at Peg Leg Street (52), Citadel Island Sewer secret (55), Principal Island Sewer secret (56), Fortress Island Secret in fortress (86) | Underground / sewers / small interiors |
| 28 | 718 B | Scene header | Proxima Island Museum (43), Hamalayi Mountains Mutation centre 2nd (68), Fortress Island outside fortress destroyed (93), Polar Island 3rd scene (107) | |
| 29 | 2 210 B | Scene header | Proxima Island near Inventor's house (44) | Single scene |
| 30 | 1 568 B | Script only | — | |
| 31 | 166 B | Code only | **Adeline Software logo screen** (hardcoded in engine) | 14-note Vibraphone jingle. Identical between MIDI_MI and MIDI_SB. |
| 32 | 21 980 B | Scene header | Polar Island end scene (114), Citadel Island end sequence 1 (116) & end sequence 2 (117) | **Finale / ending music.** Byte-for-byte copy of tracks 8 and 9. |

> **Scene 119** (Credits_List_Sequence) stores value 255 (0xFF) at the music
> byte, which twin-e reads as −1 (signed) and interprets as "no music / stop".

### Duplicates confirmed by SHA-256

| Tracks | File | Decompressed size | Relationship |
|--------|------|-------------------|--------------|
| 0 = 18 | both MI and SB | 242 B | Identical in both files |
| 8 = 9 = 32 | MI | 21 980 B | Identical |
| 8 = 9 = 32 | SB | 10 540 B | Identical |
| 24 = 25 | both MI and SB | 3 052 B | Identical in both files |
| 26, 31 | MI = SB | 322 B / 166 B | Same bytes across both HQR files |

Tracks 0 and 18 are also identical cross-file (MI[0] == SB[0], MI[18] == SB[18]).

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
