# LBA1 MIDI track reference

Index tables and notes for **Little Big Adventure 1** MIDI indices 0–32 as
played by [lba-midi-play](README.md).

All 33 tracks (indices 0–32) are present in all three HQR variants.  Sizes in
the table below are decompressed bytes from `MIDI_MI.HQR` (XMIDI);
`Midi_mi_win.hqr` sizes are roughly 30–50% larger for most tracks.

Paths such as `twin-e/src/scene.c`, `engines/twine/shared.h`, `AMBIANCE.C`,
`PERSO.C`, `PLAYFLA.C`, and `GAMEMENU.C` refer to **twin-e**, **ScummVM**, or
**original LBA1 game sources** — they are not files in the `lba-midi-play` repo.

## Background

### Names vs indices

The original game data does **not** ship a list of 33 song titles — only numeric
indices.  Engines such as **twin-e** call `music_play_midi(index, …)` with no
string labels in the HQR for “track name”.

**Philippe Vachey** released a commercial **LBA1 OST** with a **small set of
named album tracks** (on the order of nine pieces, not 33) — e.g.
[Bandcamp](https://philv.bandcamp.com/album/little-big-adventure-1-o-s-t).
Those titles describe the music in album form; mapping them **one-to-one** onto
indices `0…32` is **not** documented by Adeline and is partly a matter of
interpretation (which theme is “The Quest”, which cut is the main theme, etc.).

So: there is **no authoritative 33-entry title table** from the 1994 assets.
Informal names in fan wikis or forums are **community-derived**.  The table
below uses **scene / role** (from engine data and ScummVM’s scene IDs) as the
most concrete in-game labels you can defend without inventing a soundtrack
tracklist.

### Scene assignment

Each scene entry in SCENE.HQR carries its music track index at **byte offset 38**
of the decompressed header (verified from `twin-e/src/scene.c` `loadScene()`).
Tracks not referenced from any scene header are triggered by in-game script
events (`PLAY_MIDI` opcode) or hardcoded engine calls.  Scene names come from the
ScummVM TwinE engine's `LBA1SceneId` enum (`engines/twine/shared.h`).

### CD audio vs MIDI

`PlayMusic()` in `AMBIANCE.C` routes tracks **1–9 to the CD drive**
(`PlayCdTrack()`) when `CDEnable` is set, bypassing the MIDI files entirely for
those tracks.  Tracks 10–32 are always played from the MIDI HQR.  The "CD audio
track N" notes in the table below give the corresponding disc track numbers.

## Track index

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

## Duplicates confirmed by SHA-256

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

← [lba-midi-play README](README.md)
