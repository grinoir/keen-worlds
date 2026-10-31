/* Keen Worlds: story scenes of its own, between the games. GPL-2.0-or-later. */
#ifndef KW_STORY_SCENES_H
#define KW_STORY_SCENES_H
// Mortimer's transporter takes Keen from the Megarocket down to the sand.
// The music playing (Calidune's tune) is the caller's.
void CD_PlayArrival(void);
// Keen flies up in the strange copy of his ship and meets the Megarocket.
// Starts and stops Calidune's tune itself.
void CD_PlayEnding(void);
// After the Shikadi Station: Keen lands in Bloogton and frees Spot. Plays
// the ending's calm tune itself.
void SP_PlayRescue(void);
// After Atroxian Realm's ending: the Krile's knock-off Megarocket.
void KR_PlayNewShip(void);
// Before Meurglys III, the first time: the bounty notice for Rick Uller.
void BH_PlayBountyNotice(void);
// After the Alphamatic's ending: the council's summons, VitaCorp's bounty.
void AB_PlayMessages(void);
// Before Gnosticus IV, the first time: the Shikadi signal in the cockpit.
void K4_PlayShikadiSignal(void);
#endif
