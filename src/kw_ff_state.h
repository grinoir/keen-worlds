// Foray in the Forest: the functions of kw_ff_state.c (written with it by the
// one-time conversion of its CK_STATE.C).
#ifndef KW_FF_STATE_H
#define KW_FF_STATE_H
void FF_CalcBounds(CK_object *ob);
void FF_CustomClipRects(CK_object *ob);
void FF_SetOldClipRects(CK_object *ob);
void FF_SetDeltaClipRects(CK_object *ob);
void FF_PlayerBottomKludge(CK_object *ob);
void FF_PlayerTopKludge(CK_object *ob);
bool FF_CheckPosition(CK_object *ob);
void FF_ClipToEnds(CK_object *ob);
void FF_ClipToSides(CK_object *ob);
void FF_ClipToWalls(CK_object *ob);
void FF_FullClipToWalls(CK_object *ob);
void FF_PushObj(CK_object *ob);
void FF_ClipToSpriteSide(CK_object *push, CK_object *solid);
void FF_ClipToSpriteTop(CK_object *push, CK_object *solid);
void FF_ClipToSprite(CK_object *push, CK_object *solid, bool squish);
int16_t FF_DoActor(CK_object *ob, int16_t time);
void FF_StateMachine(CK_object *ob);
void FF_NewState(CK_object *ob, CK_action *state);
void FF_ChangeState(CK_object *ob, CK_action *state);
#endif
