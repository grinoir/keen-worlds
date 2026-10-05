/*
Keen Worlds: small, location-specific musical cues.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_MUSIC_H
#define KW_MUSIC_H

// Called once per play-loop frame after the map and Keen have updated.
void KW_UpdateMusicZone(void);
// Inside the Megarocket: start its own tune (true if it did).
bool KW_StartShipMusic(void);

// Voice clips (files in the game folder's "voice" folder). Music never
// plays under a voice: KW_Speak fades it out (waiting for the fade if asked;
// not when the music is already off), plays the clip, and the music fades
// back in once the clip is over.
void KW_Speak(const char *file, bool waitForFade);
bool KW_VoiceSpeaking(void);
// Cut the clip short (its card was put away).
void KW_StopVoice(void);
// Something is about to be said: music that would start now stays off.
void KW_VoiceComingUp(bool pending);
// Fade the music down (returns the tics that takes), then turn it off.
int KW_FadeMusicOut(void);
void KW_MusicQuiet(void);

#endif
