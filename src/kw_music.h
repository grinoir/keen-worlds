/*
Keen Worlds: small, location-specific musical cues.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_MUSIC_H
#define KW_MUSIC_H
#include <stdbool.h>

// Called once per play-loop frame after the map and Keen have updated.
void KW_UpdateMusicZone(void);
// Inside the Megarocket: start its own tune (true if it did).
bool KW_StartShipMusic(void);

// The title tune of Commander Keen: Worlds; it is not started again while it
// plays. KW_TitleMusicIsOver: other music began, or the music was stopped.
void KW_StartTitleMusic(void);
void KW_TitleMusicIsOver(void);
// The ending's calm tune, at a volume of 0 to 256.
void KW_StartEndingMusic(int volume);
// Calidune's arrival tune, the same way.
void KW_StartCaliduneMusic(int volume);
// Voice clips (files in the game folder's "voice" folder). Music never
// plays under a voice: KW_Speak fades it out (waiting for the fade if asked;
// not when the music is already off), plays the clip, and the music fades
// back in once the clip is over.
// Player preferences, independent of music/effects and saved-game state.
bool KW_VoicesEnabled(void);
int KW_VoiceVolume(void); // 0..100
void KW_SetVoicesEnabled(bool enabled);
void KW_SetVoiceVolume(int volume);
void KW_Speak(const char *file, bool waitForFade);
bool KW_VoiceSpeaking(void);
bool KW_ClipPlaying(void); // (a clip is sounding now, not merely expected)
// A line of dialogue in a game's own window: its clip, with the music turned
// down under it (only in the world the clip was made for; no clip, no
// change). KW_LinesOver: the windows are put away (the play loop does this
// too, when play goes on). KW_LinePoll: bring the
// music back up once the line is over, while a window stays up.
bool KW_SayLine(int world, const char *file); // (true: it is being spoken)
// The same, by the window's name: the clip <prefix>_<name>.RAW of the world
// Keen is in (K4_... to K9_...).
bool KW_SayWorldLine(const char *name);
void KW_LinePoll(void);
void KW_LinesOver(void);
// Is there a clip of this name (see kw_music.c: asked for as <NAME>.RAW,
// kept as <NAME>.OGG)?
bool KW_ClipIsThere(const char *file);
// A clip by itself; the music is the caller's business (true: it plays).
bool KW_Narrate(const char *file);
// The same over music that plays on: it fades away under him and comes
// back when he has finished (KW_LinePoll, KW_LinesOver).
bool KW_NarrateOverMusic(const char *file);
// Several of them as one clip, in order (those that have one).
bool KW_SayWorldLines(const char *const *names, int count);
// Cut the clip short (its card was put away).
void KW_StopVoice(void);
// Something is about to be said: music that would start now stays off.
void KW_VoiceComingUp(bool pending);
// Fade the music down (returns the tics that takes), then turn it off.
int KW_FadeMusicOut(void);
void KW_MusicQuiet(void);

#endif
