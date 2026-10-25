# Alphamatic opening: the backyard workshop

Implemented locally, 2026-10-25. The original 21-page book opening is replaced
by an abridged native scene. Original source text and endings are retained.
The user's accepted direction is a short illustrated roll, then a staged
conversation with Keen and Spot in his workshop and the others on a video call.

## Sequence and story

1. A 30-second upward credit roll fills the left half of the picture, with
   workshop, hard-drive and VitaCrypt imagery visible at right. The narrator
   explains Atroxus' salvaged drive and the missing decryption key.
2. Spot walks over while the three callers connect. Keen and Spot remain
   physically together in the foreground throughout the conversation.
3. Fourteen abridged dialogue turns establish the Krile's survival, VitaCorp's
   disappearance, the Alphamatic's construction and location near Mars, and
   the plan to disable it and steal the decryption key. The machine plans
   and Mars appear on a secondary display while the callers stay visible.
4. The Megarocket approaches the station in stealth mode. The narrator
   describes securing the Attach-O-Ray and boarding; the game then continues
   on the Alphamatic map without changing the player's progress.

This retains the original opening's location and remote call. It does not
move the friends physically to Earth or invent a different meeting location.
The complete script is in `data/mods/alphamatic/opening.json`. Narration totals
114.7 seconds; the full scene runs roughly two minutes with automatic
advancement, and can be advanced sooner during conversation.

## Presentation and controls

`src/kw_alphamatic_opening.c` renders compiled EGA workshop/drive backgrounds,
masked native character art, subtitle panels, active-speaker borders, idle
poses, Spot's approach/bounce, CRT lights and the departure animation. Images
are compiled by `kwtools/kw_alphamatic_opening.py`. Source art and the exact
built-in image generation prompts are in `../art/alphamatic/opening/` from
the workspace root. The existing mod supplies portraits/sprites and music.

Dialogue automatically advances after speech ends; with voices muted it
uses a reading delay. A fresh key/controller tap advances a dialogue turn
even during speech. The roll and connection animation ignore ordinary taps.
Holding Escape or the mapped controller menu button skips the whole opening;
held input/release cannot leak into the next turn or gameplay. The quiet
presentation hint follows the existing style. Voice on/off and volume apply.

Keen uses bm_fable at 1.2 speed, Spot af_nova with pitch 1.12, the Vorticon
am_michael, Lindsey bf_emma, and the bearded caller the user-requested elder
voice bm_george. The narrator remains bm_lewis. Sixteen new AL_CUT clips keep
the abridged opening separate from the original dialogue and ending clips.

## Local preview and verification

`play/preview-alphamatic-opening.sh` shows the installed scene in an isolated
save/config directory and writes `play/alphamatic-opening-preview.log`. It
copies the player's audio/display preferences but leaves the player's saves
and configuration untouched. This also lets an already-visited Alphamatic
be previewed without restoring story replay to the pause menu.

`sh tests/test-alphamatic-opening.sh BINARY PLAY_FOLDER` renders all stages
using the real game data and voices, checks the animated pose, and verifies
that the game state, map position and cache level survive. `/ALOPENING` is
its developer entry point. `KW_AUTOCUTSCENE=FOLDER` captures all stages without
waiting; it is only for regression rendering. Ordinary play uses real timing.

The scene renders under ASan/UBSan. Real SDL input checks confirm that an
ordinary tap during the roll is ignored, a tap advances a playing spoken
turn, and held Escape skips the entire sequence and returns cleanly.


The uninterrupted voiced run also completes and returns to the map. The
normal ship flight-console route invokes the new opening and resumes in the
cabin after landing. The installed preview wrapper was checked with an
isolated SDL run; the player's preference-file hash remained unchanged.
The full 17-world self-test and existing presentation/voice regressions pass.
Only the local executable and assets were installed; no release was packaged.


Visual cleanup after the local preview: removed the half-screen checkerboard
from the crawl (a thin letter outline preserves readability), removed the
moving top-edge scan line from the call screen, and keyed Spot's gray backing
and the station's blue guide grid/gray backing separately. Edge-connected
background is removed while enclosed detail and black outlines are retained.
The circular station platform remains. Render/state checks pass with the
actual data and voices, including ASan/UBSan; dialogue and timing are unchanged.


The subsequent text/character correction fixes the actual lettering cause:
the legacy font function XORs ink with the background, so white changed
colour over artwork and a black outline did nothing. The scene now uses
`VH_DrawPropStringSolid`, leaving legacy drawing behavior elsewhere intact.
A real-renderer check verifies white/yellow/black ink on all sixteen EGA
background colours. The crawl's black outline now paints actual black pixels.

Keen now has three dedicated 44×64 scene poses, rendered at native size rather
than enlarged from gameplay. Spot has complete 32×44 art including two feet;
a separate alpha index preserves his opaque black mouth and gray details.
Portrait background keying is retained only for the remote callers/blueprint.
Source sheets, palette conversions, previews and exact built-in imagegen
prompts are in `art/alphamatic/opening` in the workspace. Timing, narration
and dialogue are unchanged. Native rendering/state checks pass under ASan/UBSan.


The next visual pass removes the yellow active-speaker lines under Keen and
Spot. The physical flight shot now uses dedicated metallic station and
Megarocket sprites over a textured Mars backdrop, rather than the green
workshop schematic and a primitive craft. The ship decelerates into its
approach, cuts its exhaust for stealth and attaches a tether to the station.
The transition into orbit fades through black. Original dialogue and voice
clips are unchanged; the green diagram remains an intelligence briefing image.

Skip hints now start hidden and do not reappear when narration finishes or a
new turn begins. A fresh non-Escape key, or non-menu controller button,
reveals help for two seconds. Escape and the controller's skip button still
skip on a deliberate hold, silently unless help was already requested.
The same policy applies to all story presentations; popup controls retain
fresh-tap advancement with no hint.

The installed local executable passes the 22-frame opening check, including
all 16 voice clips and unchanged game state. Presentation and voice controls
pass; native SDL captures verify hidden/requested/expired/Escape-only hint
states. The complete scene render also passes ASan/UBSan.
