# lba-midi-play

A small **command-line** player for the MIDI music in **Little Big Adventure 1**
(1994): point it at `MIDI_MI.HQR`, `MIDI_SB.HQR`, or `Midi_mi_win.hqr` from your
game files and it plays a chosen track through your speakers.

---

## Quick start

**You need two things:**

1. **An HQR file from the game** (e.g. `MIDI_MI.HQR`) — copy it next to the
   built binary or pass a path to it.
2. **A General MIDI `.sf2` soundfont** — the HQR only contains *MIDI* (note
   data). A soundfont is a big file that defines what those notes *sound like*.
   Without one, there is no audio.

**Easiest path**

```sh
make
./lba-midi-play MIDI_MI.HQR 3
```

- **Linux:** install a font package first (e.g. `timgm6mb-soundfont` on
  Debian/Ubuntu — see [Soundfonts](#soundfonts)) so the tool can find a
  system-wide `.sf2` automatically.
- **macOS:** drop **`FluidR3_GM_GS.sf2`** into `/Library/Audio/Sounds/Banks/`, or
  pass any `.sf2` path as the third argument (see [Soundfonts](#soundfonts)).
- **Windows:** build with **MSYS2 MinGW** (`make` → `.exe`), then pass a `.sf2`
  path (see [Build → Windows](#windows-msys2-portable-across-drives)).

If you get **no soundfont found**, add the font explicitly:

```sh
./lba-midi-play MIDI_MI.HQR 3 ./MyFont.sf2
```

Press **Enter** to stop playback.

**More:** [Usage](#usage) · [Which HQR file?](#the-midi-files) ·
[Track index](TRACKS.md) · [How it works](#implementation-notes)

---

## Build

```sh
# Clone or copy this directory alongside your game files, then:
make
```

The Makefile picks **linker flags** from `uname`: macOS (Core Audio), Linux
(`-lpthread -ldl -lm`), **MSYS2 MinGW** (`-lpthread -lm` — no `libdl` on
Windows).  No `configure` step; only `cc`, `make`, and the vendored headers.

### Windows (MSYS2, portable across drives)

Use a **MinGW** environment (**MINGW64** or **UCRT64** in MSYS2), not the old
“MSYS”-only shell, so you get a normal Windows `.exe` and `uname` reports a
`MINGW…` system (which the Makefile recognizes).

**Path to the repo:** MSYS maps drives as `/d/...`, `/c/...`.  Examples:

```sh
# Project on D:  →  D:\lba2-hacking\lba-midi-play
cd /d/lba2-hacking/lba-midi-play

# Or under your MSYS home (often on C:), e.g. /home/User-PC/lba2-hacking/lba-midi-play
cd ~/lba2-hacking/lba-midi-play

make
./lba-midi-play.exe MIDI_MI.HQR 3 C:/path/to/some-font.sf2
```  Pass a **`.sf2` path** as the third
argument (see [Soundfonts](#soundfonts)); there are no Windows default search
paths in the binary yet.

One-time MSYS2 setup (from a MinGW shell):

```sh
pacman -S mingw-w64-x86_64-gcc make
```

Requirements: a C99 compiler (`cc` / `gcc`), `make`, and `tsf.h` / `tml.h` /
`miniaudio.h` (vendored in the repo).  Implementation: **TinySoundFont** +
**miniaudio** — no extra libraries to link beyond the OS.  Soundfonts are
loaded at runtime — see [Soundfonts](#soundfonts).

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

LBA1 exists in three MIDI variants, all 33 tracks (indices 0–32 — see
[TRACKS.md](TRACKS.md)):

| File | Version | Format | Target hardware |
|------|---------|--------|-----------------|
| `MIDI_MI.HQR` | DOS | XMIDI IFF | **General MIDI** over MPU-401 / Wave Blaster (e.g. Roland Sound Canvas SC-55) |
| `MIDI_SB.HQR` | DOS | XMIDI IFF | Sound Blaster OPL2/OPL3 FM synthesis |
| `Midi_mi_win.hqr` | LBAWin port | Native SMF | General MIDI, Windows multimedia |

The **MIDI_MI** data follows **General MIDI** (GM): fixed “program 1 = piano,
33 = bass,” etc.  Some DOS menus also offered **Roland MT-32**, but MT-32 uses
a **different** instrument list than GM, so the same file often sounds wrong on
MT-32.  A **GM** module (e.g. Roland Sound Canvas **SC-55**) matches what this
MIDI expects.

This tool plays all three.

### MIDI_MI.HQR — DOS "MIDI Interface" version

Intended for **General MIDI** playback on typical 1990s PC setups (MPU-401,
Wave Blaster, or an external module such as a Roland **Sound Canvas SC-55**).
Stored as **XMIDI IFF** (Miles Sound System format), converted to SMF on load.
Arrangements are **richer** than the FM version: more simultaneous voices,
instruments such as Fretless Bass, Orchestral Harp, Synth Brass, Piccolo, and
layered pad sounds.  File sizes are typically **larger** (e.g. track 8:
21 980 bytes vs 10 540 in MIDI_SB).

Composition-side, Vachey describes early LBA work on an **Atari 1040** with
samplers, later reworked at **Delphine Records** studios — not the same chain
as the shipped DOS GM files, but consistent with treating **MIDI_MI** as
**GM**-targeted game audio ([MO5.COM interview, 2019](https://mag.mo5.com/165588/interview-de-philippe-vachey-compositeur-de-little-big-adventure/)).

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

### Soundfonts and the three MIDI sources

There is no single “right” version to prefer — each HQR reflects a different
target (DOS GM, DOS FM, LBAWin SMF).  The interesting part of playing these
tracks today is **trying different General MIDI soundfonts**: the same MIDI
can feel very different depending on the bank, which is what this tool is
really for.

---

## Tracks

LBA1 uses **33 MIDI indices** (0–32) across the three HQR variants.  Scene
assignments, decompressed sizes, duplicate-byte relationships, CD vs MIDI
routing, and notes on **track naming** (vs Philippe Vachey’s separate OST
release) are documented in **[TRACKS.md](TRACKS.md)**.

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

LBA uses the **XMIDI** format produced by **Miles Sound System (MSS)** — John
Miles / Miles Design (later [RAD Game Tools](https://www.radgametools.com/miles.htm);
see [Wikipedia](https://en.wikipedia.org/wiki/Miles_Sound_System)).  XMIDI is an
IFF FORM container wrapping MIDI event data.

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

**Auto-discovery is intentionally small** — only a few paths where Linux and
macOS commonly install a **default General MIDI** bank.  Anything else (big
FluidR3, GeneralUser, a custom download) you pass as the **third argument**.

The tool looks for a soundfont in this order:

1. **Third argument** if you pass one: `./lba-midi-play MIDI_MI.HQR 3 my.sf2`
2. `/Library/Audio/Sounds/Banks/FluidR3_GM_GS.sf2` (macOS — if you placed that file there)
3. `/usr/share/sounds/sf2/default-GM.sf2` (Debian/Ubuntu `update-alternatives` default GM)
4. `/usr/share/sounds/sf2/TimGM6mb.sf2` (e.g. `timgm6mb-soundfont` package)
5. `/usr/share/soundfonts/default.sf2` (Arch symlink — see [Arch Linux](#linux-installing-a-soundfont) below)

If nothing matches, pass **any** `.sf2` path (absolute or relative to your shell
directory), e.g. next to the binary: `./FluidR3_GM.sf2`.

### Linux: installing a soundfont

This tool reads the `.sf2` file directly — you do **not** need FluidSynth,
TiMidity++, or a MIDI daemon.

**Easiest:** `sudo apt install timgm6mb-soundfont` on Debian/Ubuntu — you get
**TimGM6mb** and **`default-GM.sf2`** is registered via `update-alternatives`,
which matches the search list above.

**Larger / nicer banks** (FluidR3, etc.): install the package (e.g.
`fluid-soundfont-gm`) and either run `update-alternatives --config default-GM.sf2`
so **`default-GM.sf2`** points at FluidR3, **or** pass the full path to the
`.sf2` as the third argument (no need to change alternatives).

- **Arch Linux:** install a soundfont package from the [MIDI](https://wiki.archlinux.org/title/MIDI)
  / [FluidSynth](https://wiki.archlinux.org/title/FluidSynth) wikis (e.g.
  `soundfont-fluid`).  Optional: symlink your preferred bank to
  **`/usr/share/soundfonts/default.sf2`** so auto-discovery finds it without
  typing a path every time.

### macOS

Put a GM `.sf2` in **`/Library/Audio/Sounds/Banks/`** if you want auto-discovery
to find **`FluidR3_GM_GS.sf2`** (that exact name — or pass any path as the third
argument).  **`~/Library/Audio/Sounds/Banks/`** is not searched; use an
explicit path for user-local files.

### Windows

No default paths in this build — pass **`lba-midi-play … HQR index C:\path\to\font.sf2`**
(or MSYS-style paths).

### Recommended soundfonts (open / common for 90s GM games)

All are **General MIDI** — good for experimenting; LBA has no single “correct”
bank.

| Soundfont | Rough size | Why people use it |
|-----------|------------|-------------------|
| **TimGM6mb** | ~6 MB | Small, ships with many Linux distros ([Debian package](https://packages.debian.org/timgm6mb-soundfont)); fine for a quick listen. |
| **FluidR3 GM** | ~140 MB | Very common reference bank; [Debian `fluid-soundfont-gm`](https://packages.debian.org/sid/fluid-soundfont-gm), often packaged elsewhere as `FluidR3_GM.sf2`. |
| **GeneralUser GS** | ~30 MB | Popular free GM/GS bank, good for retro game MIDI — [author’s site](https://schristiancollins.com/generaluser.php). |
| **FreePats General MIDI** | varies | Fully open instrument set — [FreePats](https://freepats.zenvoid.org/SoundSets/general-midi.html). |

Pass the path to any of these as the **third argument** if they are not your
distro’s `default-GM` target.

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

### License

This project is **GNU General Public License v2.0 only** (`GPL-2.0-only`) —
see [`LICENSE`](LICENSE).  That matches the upstream terms of the adapted
**HQR** and **XMIDI** code (TwinEngine / ScummVM / Exult).  **TinySoundFont**,
**TML**, and **miniaudio** remain under their own licenses in the respective
headers (MIT or public domain); they are bundled in a way compatible with GPLv2.

### Credits

- **LBA MIDI music** — Adeline Software International / Philippe Vachey (1994)
- **TwinEngine** (HQR + XMIDI code) — The TwinEngine team (GPL v2)
- **XMIDI conversion** — Originally ScummVM / Exult projects (GPL v2)
- **TinySoundFont / TML** — Bernhard Schelling (MIT)
- **miniaudio** — David Reid (MIT / public domain)
