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

#endif
