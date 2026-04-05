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

# Play the LBAWin Windows port version (native SMF, richer arrangements)
./lba-midi-play Midi_mi_win.hqr 3
```

Press **Enter** to stop playback.

---

## The MIDI Files

LBA1 exists in three MIDI variants, all 33 tracks (indices 0–32):

| File | Version | Format | Target hardware |
|------|---------|--------|-----------------|
| `MIDI_MI.HQR` | DOS | XMIDI IFF | Roland MT-32 / Sound Canvas SC-55 or any GM MIDI port |
| `MIDI_SB.HQR` | DOS | XMIDI IFF | Sound Blaster OPL2/OPL3 FM synthesis |
| `Midi_mi_win.hqr` | LBAWin port | Native SMF | General MIDI, Windows multimedia |

This tool plays all three.

### MIDI_MI.HQR — DOS "MIDI Interface" version

Intended for an external MIDI port driving a Roland Sound Canvas SC-55 (the
canonical LBA instrument — what the composers heard).  Stored as **XMIDI IFF**
(Miles Sound System format), converted to SMF on load.  Arrangements are
**richer**: more simultaneous voices, instruments such as Fretless Bass,
Orchestral Harp, Synth Brass, Piccolo, and layered pad sounds.  File sizes
are typically **larger** (e.g. track 8: 21 980 bytes vs 10 540 in MIDI_SB).

### MIDI_SB.HQR — DOS "Sound Blaster FM" version

Intended for the OPL2/OPL3 FM synthesis chip on Sound Blaster cards.  The
original source (`PERSO.C`) selects `midi_sb.hqr` when `MidiFM` is set (FM
driver active) and `midi_mi.hqr` otherwise.  Arrangements are **simplified**:
fewer simultaneous voices, simpler instrument choices.  OPL FM chips have
limited polyphony and cannot reproduce complex pads or layered strings, so
tracks were re-arranged accordingly.

### Midi_mi_win.hqr — LBAWin port version

Shipped with the **LBAWin** Windows port (dated 2001), created by
**Sébastien Viannay** — an original Adeline Software developer (story coding
on LBA1, programming on LBA2).  Not a commercial release; distributed via
the Magicball Network (`magicball.net`).  The GOG and Steam releases ship
the DOS files only; `Midi_mi_win.hqr` must be sourced from LBAWin separately.
This file is **fundamentally different** from the DOS HQR files:

- **Native SMF** (Standard MIDI File) — no XMIDI wrapping, no conversion needed
- **SMF Format 1** (multi-track) with up to 14 separate instrument tracks per song
- **Higher PPQN**: mostly 384 (some tracks 96 or 120) vs 60 for XMIDI-converted DOS
- **Real tempo events** encoded per-track (64–133 BPM range) vs forced 120 BPM
- **Longer arrangements**: most tracks are 30–50% larger in decompressed size
- **Tracks 24/25 swapped**: the DOS version had a unique ~3 kB piece for 24/25;
  the WIN version replaced both with a copy of track 3

Track 31 (Adeline logo jingle) is slightly shorter in WIN (139 vs 166 bytes)
and track 26 (FLA flute) is nearly identical.

### Which sounds better through a modern software synthesiser?

With a high-quality General MIDI soundfont **`Midi_mi_win.hqr`** is often the
most musically complete option: higher timing resolution, proper multi-track
layout, and richer arrangements.  **`MIDI_MI.HQR`** (XMIDI) is historically
what DOS players heard through a General MIDI port.  `MIDI_SB.HQR` is closest
to what most players actually experienced on typical mid-90s PC hardware.

---

## Track List

All 33 tracks (indices 0–32) are present in all three files.  Sizes in the
table below are decompressed bytes from `MIDI_MI.HQR` (XMIDI); `Midi_mi_win.hqr`
sizes are roughly 30–50% larger for most tracks.

**How track assignment works:** each scene entry in SCENE.HQR carries its
music track index at **byte offset 38** of the decompressed header (verified
from `twin-e/src/scene.c` `loadScene()`).  Tracks not referenced from any
scene header are triggered by in-game script events (`PLAY_MIDI` opcode) or
hardcoded engine calls.  Scene names come from the ScummVM TwinE engine's
`LBA1SceneId` enum (`engines/twine/shared.h`).

**CD audio:** `PlayMusic()` in `AMBIANCE.C` routes tracks **1–9 to the CD
drive** (`PlayCdTrack()`) when `CDEnable` is set, bypassing the MIDI files
entirely for those tracks.  Tracks 10–32 are always played from the MIDI HQR.
The "CD audio track N" notes in the table below give the corresponding disc
track numbers.

| Idx | MI size | Trigger | Scene / context | Notes |
|-----|---------|---------|-----------------|-------|
| 0 | 242 B | Script only | — | Single Recorder note; identical to track 18. Never triggered from a scene header. |
| 1 | 7 274 B | Scene header + code | Principal Island Ruins (17), White Leaf Desert Maze (57), Brundle Island teleportation (95, 99); **intro dream sequence** — played immediately before INTROD.FLA (hardcoded) | CD audio track 2 when CD enabled |
| 2 | 8 104 B | Scene header | Principal Island Harbor (11), Ticket Office (22), White Leaf Desert Temple 2nd (41), Hamalayi Mountains fighting scenes (62, 63, 69), Polar Island rocky peak (110, 111) | Action / battle theme |
| 3 | 8 612 B | Scene header | Rebellion Island Harbor (59), Rebellion Island Rebel camp (60) | |
| 4 | 1 768 B | Scene header | Citadel Island Warehouse (35), White Leaf Desert outside Temple of Bu (36), Fortress Island outside (84), Fortress Island Docks (100) | |
| 5 | 5 738 B | Scene header | Citadel Island near tavern (2) & pharmacy (3), White Leaf Desert Temple 1st (8) & 2nd (40), Hamalayi Mountains Prison (64), Fortress Island Secret passage (85), Principal Island inside fortress (105) | |
| 6 | 3 422 B | Scene header + code | Citadel Island Twinsen's house (5), Principal Island Library (10), Old Burg (13), inside Rabbibunny house (28), Stables (32), House with TV (58), Tippet Island Village (74); **DEMO version end credits** (hardcoded) | CD audio track 7 when CD enabled |
| 7 | 9 394 B | Scene header | Hamalayi Mountains landing place (9), Mutation centre 1st (67), Catamaran dock (72) | |
| 8 | 21 980 B | Script only | — | Identical to tracks 9 and 32 (SHA256 confirmed). Never triggered from a scene header — played via script events. |
| 9 | 21 980 B | Scene header + code | Principal Island Port Belooga (24), Tippet Island near Dino-Fly (78), Hamalayi Mountains Ski resort (96), Principal Island house in Port Belooga (102); **Options Menu** (hardcoded in `GAMEMENU.C` `OptionsMenu()`, non-CD) + **credits loop** (hardcoded in `PERSO.C`) | Byte-for-byte copy of track 8. CD audio track 10 when CD enabled |
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
| 26 | 322 B | Code only | **FLA cutscene flute music** (hardcoded in `PLAYFLA.C`, comment `// fla flute`) | Identical between MIDI_MI and MIDI_SB |
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
| 0 = 18 | DOS MI and SB | 242 B | Identical in both DOS files |
| 8 = 9 = 32 | DOS MI | 21 980 B | Identical |
| 8 = 9 = 32 | DOS SB | 10 540 B | Identical |
| 24 = 25 | DOS MI and SB | 3 052 B | Identical pair (unique content, not in WIN) |
| 26, 31 | DOS MI = SB | 322 B / 166 B | Same bytes across DOS files |
| 0 = 18 | WIN | 225 B | Identical (new arrangement) |
| 3 = 24 = 25 | WIN | 12 343 B | WIN tracks 24/25 replaced with track 3's content |
| 8 = 9 = 32 | WIN | 29 833 B | Identical (expanded arrangement) |

Tracks 0 and 18 are also identical cross-file (MI[0] == SB[0], MI[18] == SB[18]).
The DOS-only 3 052-byte piece in tracks 24/25 does not appear anywhere in the WIN file.

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

All MIDI entries in the DOS HQR files use compression mode 1.
The compression ratio is roughly 2:1 (e.g. MIDI_MI track 3: 8 612 bytes real,
4 790 bytes stored).  `Midi_mi_win.hqr` also uses mode 1 for most entries;
track 31 and track 26 use mode 0 (uncompressed) in both DOS and WIN.

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
2. `/Library/Audio/Sounds/Banks/FluidR3_GM_GS.sf2`
3. `/Library/Audio/Sounds/Banks/Roland_SC-55_v3.7.sf2`
4. `/usr/share/sounds/sf2/FluidR3_GM.sf2`
5. `/usr/share/soundfonts/FluidR3_GM.sf2`
6. `/usr/share/sounds/sf2/TimGM6mb.sf2`

If none of these are present, pass a path explicitly.

### Recommended soundfonts

| Soundfont | Size | Notes |
|-----------|------|-------|
| **FluidR3_GM.sf2** | 144 MB | Excellent all-rounder; widely available |
| **GeneralUser GS** | ~30 MB | Lightweight, good GM coverage — [download](https://schristiancollins.com/generaluser.php) |
| **Roland SC-55 samples** | varies | Closest to the original composers' monitor; source your own SF2 |
| **TimGM6mb.sf2** | 6 MB | Tiny, usable, common on Linux |
| ~~MuseScore_General.sf3~~ | — | SF3 format — not supported by TSF, which requires SF2 |

### The Roland SC-55 note

LBA's music was composed and arranged specifically for the **Roland Sound
Canvas SC-55** — the gold standard consumer GM synth of 1994.  If you have
access to a Roland SC-55 sample library in SF2 format, that is the most
historically accurate playback.

```sh
./lba-midi-play MIDI_MI.HQR 3 /path/to/roland-sc55.sf2
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

The tool auto-detects format by checking the first four bytes of each entry
(`FORM` = XMIDI, `MThd` = native SMF):

```
HQR file (on disk)
      │  hqr_get_entry_alloc()  — LZ decompress
      ▼
raw bytes (XMIDI or SMF)
      │  if XMIDI: convert_to_midi() — XMIDI IFF → SMF format 0
      │  if SMF:   used directly     (Midi_mi_win.hqr)
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
