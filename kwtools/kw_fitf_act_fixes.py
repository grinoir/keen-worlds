"""One-time hand fixes on the converted CK_ACT.C (kw_ff_act.c), applied after tools/kw_fitf_port.py."""
import re, sys
p = sys.argv[1]; t = open(p).read()
def sub(a, b, count=0, flags=0):
    global t
    t, n = re.subn(a, b, t, count=count, flags=flags)
    if not n: print('no match:', a[:60])
# extra sprites kept in temp fields
sub(r'RF_AddSpriteDraw\(&(\w+)->user(\d)', r'RF_AddSpriteDrawUsing16BitOffset(&\1->user\2')
sub(r'RF_RemoveSpriteDraw\(&(\w+)->user(\d)\)', r'RF_RemoveSpriteDrawUsing16BitOffset(&\1->user\2)')
# object pointers kept in temp fields
sub(r'\(int16_t\)\s*(best|b|ob|hit|clone)\b', r'FF_ObjToUser(\1)')
sub(r'\(\s*CK_object\s*\*\s*\)\s*(\w+->user\d)', r'FF_ObjFromUser(\1)')
sub(r'\(int16_t\)FF_SpawnSlideDoor\(([^;]*)\);', r'FF_ObjToUser(FF_SpawnSlideDoor(\1));')
# map access by offset
sub(r'mapbwidthtable\[([^\]]+)\]\s*/\s*2', r'((\1) * CA_GetMapWidth())')
sub(r'mapsegs\[(\d)\]\[([^\]]+)\]', r'FF_MapAt(\1, \2)')
sub(r'\bRF_MemToMap\b', 'RF_ReplaceTiles')
sub(r'spritetable\[([^\]]+)\]\.orgy', r'VH_GetSpriteTableEntry(\1).originY')
sub(r'spritetable\[([^\]]+)\]\.yh', r'VH_GetSpriteTableEntry(\1).yh')
sub(r'\bbonuspoints\b', 'ck_itemPoints')
sub(r'if \(objfreelist\)', 'if (true)')
sub(r'new_object == &dummyobj', 'FF_ObjToUser(new_object) == FF_ObjToUser(NULL) /* no free object */')
sub(r'\bFF_Quit\(', 'Quit(')
sub(r'\bcl_custommidclip\b', 'FF_CLIP_custommid')
sub(r'\bs_Wormouth\.deltaPosX\b', 'CK_ACTION(FF_ACT_Wormouth)->velX')
sub(r'ck_keenState\.upheld', 'ff_keenUpHeld')
sub(r'^\s*brainfucled\+\+;', '\tff_brainfucled++;', flags=re.M)
sub(r'turn &= 7;', 'turn = (turntype)(turn & 7);')
sub(r'#if \(G_T_SHIFT == 8\) && \(sizeof\(ob->user4\) == 2\)', '#if 1')
sub(r'#if \(\(int16_t\)\(MUSHROOMDELAY\) > 0\)\n#error[^\n]*\n#endif\n', '')
sub(r'^\tCK_ACT_Add(?:Col)?Function\("FF_(?:SmirkyCheckTiles|T_YorpSearch|T_YorpWalk|C_Yorp)"[^\n]*\n', '', flags=re.M)
# sprites drawn flipped, clipped or in a fixed order: our own draw calls
sub(r'RF_AddSpriteDraw\((&ob->sde, [^;]*?), \(ob->user2 & 4\)\? false\|drawflipped : false, ob->zLayer\)', r'FF_DrawSprite(\1, (ob->user2 & 4) ? drawflipped : 0, ob->zLayer)')
sub(r'RF_AddSpriteDraw\((&ob->sde, [^;]*?), false\|drawflipped, ob->zLayer\)', r'FF_DrawSprite(\1, drawflipped, ob->zLayer)')
sub(r'RF_AddSpriteDraw\((&ob->sde, [^;]*?), ob->user1\|false, ob->zLayer\)', r'FF_DrawSprite(\1, ob->user1, ob->zLayer)')
# forward declarations of everything the file defines
body = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/', ' ', t)
protos = []
for m in re.finditer(r'^((?:static\s+)?[A-Za-z_][\w \t\*]*?\b(FF_\w+)\s*\([^;{}]*\))\s*\n\{', body, re.M):
    sig = ' '.join(m.group(1).split())
    if sig not in protos: protos.append(sig)
t = t.replace('#include "kw_fitf.h"\n', '#include "kw_fitf.h"\n#include "kw_ff_keen.h"\n\ntypedef int turntype_fwd;\n' + ''.join(s + ';\n' for s in protos if 'turntype' not in s and 'detectorinfo' not in s), 1)
open(p, 'w').write(t)
print(len(protos), 'prototypes')

# ---- second round (by hand) ----
t = open(p).read()
def rep(a, b):
    global t
    if a not in t: print('no match:', a[:60])
    t = t.replace(a, b)
rep("""	uint16_t  *map;

""", "\n")
rep("""	map = &FORESPOT(ob->posX, ob->posY);
	for (i = 0; i < ob->user1; i++, map+=CA_GetMapWidth())
	{
		tilesToReplace[i] = *map + ob->user2;""", """	for (i = 0; i < ob->user1; i++)
	{
		tilesToReplace[i] = FORESPOT(ob->posX, ob->posY + i) + ob->user2;""")
rep("	addr = FP_OFF(&new_object->sde);\n", "	addr = FF_ObjToUser(new_object); // KEEN WORLDS: the janitor, whose sprite the bucket is drawn after\n")
# its sprites drawn straight after another (RF_PlaceSpriteAfter)
rep('\t\tRF_PlaceSpriteAfter(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, &ob->user3);\n', '\t\tFF_PlaceSpriteAfterOffset(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, &ob->user3);\n')
rep('\tRF_PlaceSpriteAfter(&(ob->sde), ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, ob->user1);\n', '\tRF_PlaceSpriteAfter(&(ob->sde), ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, ob->user1 ? &FF_ObjFromUser(ob->user1)->sde : NULL); // KEEN WORLDS: the janitor, by its place among the objects\n')
rep("""	uint16_t temp;
	// Make Nisasi let go (if creature is grabbed by a Nisasi
	
	// check if temp4 value is a pointer to an entry in the object array
	temp = ob->user4 - FP_OFF(&objarray);
	if (temp < (uint16_t)sizeof(objarray) && temp % sizeof(CK_object) == 0)
	{
		CK_object *n = FF_ObjFromUser(ob->user4);
		if (n->type""", """	// Make Nisasi let go (if creature is grabbed by a Nisasi
	
	// check if temp4 value is a pointer to an entry in the object array
	{
		CK_object *n = FF_ObjFromUser(ob->user4);
		if (n && n->type""")
# KEEN WORLDS: the Crab keeps the action to go on with in temp3; here a number
rep("ob->user3 = (int16_t)CK_ACTION(FF_ACT_CrabWall);", "ob->user3 = 0;")
rep("ob->user3 = (int16_t)CK_ACTION(FF_ACT_CrabFloor);", "ob->user3 = 1;")
rep("ob->user3 = (int16_t)CK_ACTION(FF_ACT_CrabCeiling);", "ob->user3 = 2;")
rep("CK_SetAction2(ob, (CK_action *)ob->user3);", "CK_SetAction2(ob, ob->user3 == 0 ? CK_ACTION(FF_ACT_CrabWall) : ob->user3 == 1 ? CK_ACTION(FF_ACT_CrabFloor) : CK_ACTION(FF_ACT_CrabCeiling));")
open(p, 'w').write(t)

# ---- third round: found by running ----
t = open(p).read()
# KEEN WORLDS: no checkpoint touched yet: the source reads through a null
# pointer here (harmless in DOS); keep chasing Keen instead.
rep("""			target = ff_map.lastCheckpoint;
			dy = 30*16;	// because of different sprite offsets""", """			if (ff_map.lastCheckpoint) // KEEN WORLDS: null until a checkpoint is touched
			{
			target = ff_map.lastCheckpoint;
			dy = 30*16;	// because of different sprite offsets
			}""")
open(p, 'w').write(t)

t = open(p).read()
t = t.replace('ff_map.riding', 'ck_keenState.platform')
open(p, 'w').write(t)

# ---- Keen Worlds: both Keen sprite sets at run time ----
# The source builds two programs: one for its own Keen sprites and one for
# the "classic" ones (CLASSICEXT). Here the variable ff_classicKeen (FITF.CKS)
# says which graphics file is in use, so both sets of offsets stay in.
t = open(p).read()
i = t.index('#ifdef CLASSICEXT')
j = t.index('\n', i) + 1
k = t.index('#else\t\t/* for the Xky-Keen sprites: */', j)
e = t.index('#endif\n', k)
classic = t[j:k]
own = t[t.index('\n', k) + 1:e]
a = classic.index('\t\tcase CT_Player:')
b = classic.index('\n', a) + 1
classic_body = classic[b:classic.rindex('\t\tbreak;')]
c = own.index('\n', own.index('\t\tcase CT_Player:')) + 1
own_body = own[c:own.rindex('\t\t\tbreak;')]
t = (t[:i] + classic[:a] + '\t\tcase CT_Player:\n\t\t\tif (CK_INT(ff_classicKeen, 0))\n\t\t\t{\n\t\t\t// compatible with the Vanilla-Keen sprites\n'
     + classic_body + '\t\t\t}\n\t\t\telse\n\t\t\t{\n\t\t\t// compatible with the XkyKeen sprites\n' + own_body + '\t\t\t}\n\t\t\tbreak;\n' + t[e + len('#endif\n'):])
open(p, 'w').write(t)
