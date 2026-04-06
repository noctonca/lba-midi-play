# lba-midi-play

Small command-line player for the MIDI music in Little Big Adventure 1 (1994):
point it at `MIDI_MI.HQR`, `MIDI_SB.HQR`, or `Midi_mi_win.hqr` from your game
files and it plays a chosen track through your speakers.

---

## Quick start

1. An HQR file from the game (e.g. `MIDI_MI.HQR`) — next to the binary or pass a
   path.
2. A General MIDI `.sf2` soundfont. The HQR only holds MIDI (note data); the
   soundfont defines the instruments. Without a font there is no audio.

```sh
make
./lba-midi-play MIDI_MI.HQR 3
```

- Linux: install a font package (e.g. `timgm6mb-soundfont` on Debian/Ubuntu —
  see [Soundfonts](#soundfonts)) so a system `.sf2` can be found automatically.
- macOS: put `FluidR3_GM_GS.sf2` in `/Library/Audio/Sounds/Banks/`, or pass any
  `.sf2` as the third argument ([Soundfonts](#soundfonts)).
- Windows: build with MSYS2 MinGW (`make` → `.exe`), then pass a `.sf2` path
  ([Build → Windows](#windows-msys2-portable-across-drives)).

If the program prints that no soundfont was found, pass the font explicitly:

```sh
./lba-midi-play MIDI_MI.HQR 3 ./MyFont.sf2
```

Playback exits when the track ends; press Enter to stop early.

[Usage](#usage) · [Which HQR file?](#the-midi-files) · [Track index](TRACKS.md)
· [How it works](#implementation-notes)

---

## Build

```sh
# Clone or copy this directory alongside your game files, then:
make
```

The Makefile picks linker flags from `uname`: macOS (Core Audio), Linux
(`-lpthread -ldl -lm`), MSYS2 MinGW (`-lpthread -lm` — no `libdl` on Windows).
No `configure`; only `cc`, `make`, and the vendored headers.

### Windows (MSYS2, portable across drives)

Use a MinGW environment (MINGW64 or UCRT64 in MSYS2), not the old MSYS-only
shell, so you get a normal Windows `.exe` and `uname` reports `MINGW…` (what
the Makefile expects).

MSYS maps drives as `/d/...`, `/c/...`. Examples:

```sh
# Project on D:  →  D:\lba2-hacking\lba-midi-play
cd /d/lba2-hacking/lba-midi-play

# Or under your MSYS home (often on C:), e.g. /home/User-PC/lba2-hacking/lba-midi-play
cd ~/lba2-hacking/lba-midi-play

make
./lba-midi-play.exe MIDI_MI.HQR 3 C:/path/to/some-font.sf2
```

Pass a `.sf2` path as the third argument ([Soundfonts](#soundfonts)); this build
does not search default paths on Windows.

One-time MSYS2 setup (from a MinGW shell):

```sh
pacman -S mingw-w64-x86_64-gcc make
```

Requirements: a C99 compiler (`cc` / `gcc`), `make`, and `tsf.h` / `tml.h` /
`miniaudio.h` (vendored). TinySoundFont and miniaudio — nothing else to link
beyond the OS. Soundfonts load at runtime; see [Soundfonts](#soundfonts).

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

Playback exits when the track ends; press Enter to stop early.

---

## The MIDI Files

LBA1 exists in three MIDI variants, all 33 tracks (indices 0–32 — see
[TRACKS.md](TRACKS.md)):

| File | Version | Format | Target hardware |
|------|---------|--------|-----------------|
| `MIDI_MI.HQR` | DOS | XMIDI IFF | General MIDI over MPU-401 / Wave Blaster (e.g. Roland Sound Canvas SC-55) |
| `MIDI_SB.HQR` | DOS | XMIDI IFF | Sound Blaster OPL2/OPL3 FM synthesis |
| `Midi_mi_win.hqr` | LBAWin port | Native SMF | General MIDI, Windows multimedia |

For the two DOS HQRs, `MIDI_MI` is the full General MIDI soundtrack — aimed at
MPU-401 / Wave Blaster / external GM hardware. `MIDI_SB` is the Sound Blaster FM
version: simplified arrangements for the OPL chip on typical machines. Same
music in spirit; MI is the less-constrained arrangement, SB the hardware
compromise.

`MIDI_MI` follows General MIDI (GM): fixed program numbers (e.g. piano, bass).
Some DOS menus offered Roland MT-32, but MT-32 uses a different instrument map
than GM, so the same file often sounds wrong there. A GM module (e.g. Roland
SC-55) matches what this MIDI expects.

This tool plays all three.

### MIDI_MI.HQR — DOS "MIDI Interface" version

Intended for General MIDI on typical 1990s setups (MPU-401, Wave Blaster, or an
external module such as Roland Sound Canvas SC-55). Stored as XMIDI IFF (Miles),
converted to SMF on load. Richer than the FM version: more voices, instruments
like fretless bass, harp, synth brass, piccolo, layered pads. Files are usually
larger (e.g. track 8: 21 980 bytes vs 10 540 in MIDI_SB).

Vachey described early LBA work on an Atari 1040 with samplers, later studio work
at Delphine Records — not the same pipeline as the shipped DOS GM files, but in
line with MIDI_MI as GM-targeted game audio
([MO5.COM interview, 2019](https://mag.mo5.com/165588/interview-de-philippe-vachey-compositeur-de-little-big-adventure/)).

### MIDI_SB.HQR — DOS "Sound Blaster FM" version

Intended for the OPL2/OPL3 FM synthesis chip on Sound Blaster cards.  The
original source (`PERSO.C`) selects `midi_sb.hqr` when `MidiFM` is set (FM
driver active) and `midi_mi.hqr` otherwise. Arrangements are simplified: fewer
voices, simpler patches. OPL FM chips have
limited polyphony and cannot reproduce complex pads or layered strings, so
tracks were re-arranged accordingly.

### Midi_mi_win.hqr — LBAWin port version

Shipped with the LBAWin Windows port (dated 2001), by Sébastien Viannay (Adeline
— story coding on LBA1, programming on LBA2). Not a commercial release;
distributed via Magicball Network (`magicball.net`). GOG and Steam ship the DOS
files only; `Midi_mi_win.hqr` comes from LBAWin separately. It differs from the
DOS HQRs:

- Native SMF — no XMIDI wrapping
- SMF format 1 (multi-track), up to 14 instrument tracks per song
- Higher PPQN: mostly 384 (some 96 or 120) vs 60 for converted DOS XMIDI
- Real tempo per track (about 64–133 BPM) vs forced 120 BPM in this tool’s DOS path
- Longer arrangements: most tracks ~30–50% larger decompressed
- Tracks 24/25: DOS had a unique ~3 kB pair; WIN replaced both with a copy of track 3

Track 31 (Adeline logo jingle) is slightly shorter in WIN (139 vs 166 bytes)
and track 26 (FLA flute) is nearly identical.

### Soundfonts and the three MIDI sources

There is no single “right” soundfont for the GM HQRs; swapping `.sf2` files
changes the result a lot. That is separate from which HQR you open: `MIDI_MI`
and `MIDI_SB` are different arrangements (GM vs FM-targeted), as above.

Playing `MIDI_MI` or `Midi_mi_win` today is largely about trying different GM
soundfonts — same MIDI, different bank, different character.

---

## Tracks

LBA1 uses 33 MIDI indices (0–32) across the three HQR variants. Scene
assignments, sizes, duplicates, CD vs MIDI routing, and naming caveats (vs
Philippe Vachey’s separate OST) are in [TRACKS.md](TRACKS.md).

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

LBA uses XMIDI from Miles Sound System (MSS) — John Miles / Miles Design (later
[RAD Game Tools](https://www.radgametools.com/miles.htm);
[Wikipedia](https://en.wikipedia.org/wiki/Miles_Sound_System)). XMIDI is an IFF
FORM container around MIDI event data.

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

The converter (`xmidi.c`, from ScummVM/Exult via TwinEngine) outputs SMF format 0
at 60 PPQN, tempo 500 000 µs/beat (= 120 BPM).

---

## Soundfonts

Auto-discovery only checks a few paths where Linux and macOS often put a default
GM bank. Anything else (FluidR3, GeneralUser, a download) — pass it as the third
argument.

Search order:

1. Third argument if given: `./lba-midi-play MIDI_MI.HQR 3 my.sf2`
2. `/Library/Audio/Sounds/Banks/FluidR3_GM_GS.sf2` (macOS — if you placed that file there)
3. `/usr/share/sounds/sf2/default-GM.sf2` (Debian/Ubuntu `update-alternatives` default GM)
4. `/usr/share/sounds/sf2/TimGM6mb.sf2` (e.g. `timgm6mb-soundfont` package)
5. `/usr/share/soundfonts/default.sf2` (Arch symlink — see [Arch Linux](#linux-installing-a-soundfont) below)

If nothing matches, pass any `.sf2` path (absolute or relative), e.g.
`./FluidR3_GM.sf2` next to the binary.

### Linux: installing a soundfont

The program reads `.sf2` directly — no FluidSynth, TiMidity++, or MIDI daemon.

On Debian/Ubuntu, `sudo apt install timgm6mb-soundfont` installs TimGM6mb and
registers `default-GM.sf2` via `update-alternatives`, which matches the search
list above.

Larger banks (FluidR3, etc.): install the package (e.g. `fluid-soundfont-gm`)
and either point `update-alternatives --config default-GM.sf2` at FluidR3 or
pass the full path to the `.sf2` as the third argument.

Arch: see [MIDI](https://wiki.archlinux.org/title/MIDI) /
[FluidSynth](https://wiki.archlinux.org/title/FluidSynth) wikis (e.g.
`soundfont-fluid`). You can symlink a preferred bank to
`/usr/share/soundfonts/default.sf2` so auto-discovery finds it.

### macOS

Put a GM `.sf2` in `/Library/Audio/Sounds/Banks/` as `FluidR3_GM_GS.sf2` for
auto-discovery, or pass any path as the third argument. `~/Library/Audio/...`
is not searched.

### Windows

No default paths — `lba-midi-play … HQR index C:\path\to\font.sf2` (or MSYS
paths).

### Soundfonts people often use (all General MIDI)

| Soundfont | Rough size | Notes |
|-----------|------------|-------|
| TimGM6mb | ~6 MB | Small; many Linux distros ([Debian](https://packages.debian.org/timgm6mb-soundfont)). |
| FluidR3 GM | ~140 MB | Common reference; [Debian](https://packages.debian.org/sid/fluid-soundfont-gm), often `FluidR3_GM.sf2` elsewhere. |
| GeneralUser GS | ~30 MB | Free GM/GS; [author’s site](https://schristiancollins.com/generaluser.php). |
| FreePats General MIDI | varies | Open instruments — [FreePats](https://freepats.zenvoid.org/SoundSets/general-midi.html). |

Pass the path as the third argument if it is not your distro’s `default-GM`
target.

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

The callback renders audio in blocks between MIDI events: when the internal
clock reaches `event->time`, it dispatches to TinySoundFont, then renders samples
up to the next event (or the end of the buffer).

### License

GPL-2.0-only — see [`LICENSE`](LICENSE). Same spirit as the adapted HQR and
XMIDI code (TwinEngine / ScummVM / Exult). TinySoundFont, TML, and miniaudio
keep their own licenses in the headers (MIT or public domain); bundled in a
GPLv2-compatible way.

### Credits

- LBA MIDI music — Adeline Software International / Philippe Vachey (1994)
- TwinEngine (HQR + XMIDI code) — The TwinEngine team (GPL v2)
- XMIDI conversion — ScummVM / Exult lineage (GPL v2)
- TinySoundFont / TML — Bernhard Schelling (MIT)
- miniaudio — David Reid (MIT / public domain)
