# Keen Dreams Gold Edition, native companion

**Game:** Keen Dreams Gold Edition by Szemigi, release of 2026-05-31
(`KDGOLD.ZIP`, https://pckf.com/viewtopic.php?t=18318). Its source is in the
release (`KDGSRC/`) under the GNU GPL v2; see `LICENSE`. Gold itself builds on
Keen Dreams 1.92 and code from Catacomb 3-D, Wolfenstein 3-D, K1n9_Duk3's
Keen 4-6 reconstruction, Foray in the Forest, Atroxian Realm, Reconstructed
BioMenace and NY00123's Keen Dreams tech demo (see each file's header).

**Platform layer:** David Gow's SDL2 port of Keen Dreams
(https://github.com/sulix/keen-dreams-sdl2, branch `steam`, commit `7d70426`),
GPL v2 or later: emulated EGA memory as one byte per pixel, C versions of the
assembly, SDL input.

**Keen Worlds changes (2026-10-30):**

- Gold's game code (`kdg_*.c`) as released, with the SDL2 port's portability
  fixes applied by hand: map pointers are `uint16_t`, the score box draws into
  one-byte-per-pixel sprites, the map scan is C.
- Object and state pointers that DOS kept in 16-bit fields (the flower power,
  the zees, saved games) are numbers: `kdg_states.c`.
- Gold's uncompressed data loader (`id_ca.c`) reads `KDGOLD.EGA/.MAP/.AUD`
  from the data folder and the headers DOS links into `KDGOLD.EXE` from the
  release's `KDGSRC/static/`.
- `id_sd.c`: Gold's sound manager with the hardware emulated: timer 0 runs in
  the audio callback, PC speaker square wave, AdLib through DBOPL (shared with
  Omnispeak, `src/opl/`). TimeCount follows the wall clock at 70 Hz.
- `id_vw_sdl.c`: SDL's renderer instead of the port's OpenGL.
- `id_mm.c`: the port's malloc manager; nothing is purged while playing.
- DOS text-mode screens (intro, memory report, exit screen) are not shown.
- Busy waits that relied on the keyboard interrupt pump SDL's events.
- Saves and `KDGOLD.CFG` go to the folder Keen Worlds names. Saves are this
  build's own format, not DOS Gold's.

Run as `keen-dreams-gold DATADIR SAVEDIR [-fullscreen=true|false]`. Keen Worlds
starts it from the Megarocket's bunk (`src/kw_arcade.c`).
