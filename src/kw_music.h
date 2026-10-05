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
// The ship's tune is off until the ship computer has finished speaking.
void KW_HoldShipMusic(void);
// Fade the ship's tune down (returns how many tics that takes), then, after
// waiting, turn it off until the computer has spoken; it fades back in.
int KW_FadeShipMusicOut(void);
void KW_ShipMusicQuiet(void);

#endif
