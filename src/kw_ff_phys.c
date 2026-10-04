/*
Keen Worlds: Foray in the Forest's physics helpers, ported from CK_STATE.C of
its source (Nisaba and K1n9_Duk3; GPL v2 or later). Its gravity, friction and
acceleration differ from Keen 5's, so its creatures use these.
*/

#include "kw_fitf.h"

/*
====================
=
= DoEasyKeenGravity		(K1n9_Duk3 addition)
=
= Special gravity for Keen in easy mode (between DoGravity and DoWeakGravity)
=
====================
*/

void FF_DoEasyKeenGravity(CK_object *ob)
{
	// It's safe to use int variables instead of long's in the for-loop, since
	// the code in the loop is only using the two least significant bits anyway.
	// We just need to make sure to use the condition "!=" instead of "<".
	int tickCount;
	int lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount != lastTimeCount; tickCount++)
	{
		 int amount = 2;
		// Once in every 4 tics...
		if ((tickCount & 3) == 0)
		{
			amount--;
		}
		if (ob->velY < 0 && ob->velY >= -amount)
		{
			ck_nextY += ob->velY;
			ob->velY = 0;
			return;
		}
		
		ob->velY += amount;
		if (ob->velY > MAXFALLSPEED)
		{
			ob->velY = MAXFALLSPEED;
		}
		ck_nextY += ob->velY;
	}
}

/*
====================
=
= DoGravity
=
====================
*/

void FF_DoGravity(CK_object *ob)
{
#if 0
	// original code:
	long tickCount;
	long lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
#else
	// optimized FITF code:
	int tickCount;
	int lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount != lastTimeCount; tickCount++)
#endif
	{
		// Every odd tic...
		/*if (tickCount & 1)*/
		{
			if (ob->velY < 0 && ob->velY >= -2/*-4*/)
			{
				ck_nextY += ob->velY;
				ob->velY = 0;
				return;
			}
			ob->velY += 2/*4*/;
			if (ob->velY > MAXFALLSPEED)
			{
				ob->velY = MAXFALLSPEED;
			}
		}
		ck_nextY += ob->velY;
	}
}

/*
====================
=
= DoWeakGravity
=
====================
*/

void FF_DoWeakGravity(CK_object *ob)
{
#if 0
	// original code:
	long tickCount;
	long lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
#else
	// optimized FITF code:
	int tickCount;
	int lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount != lastTimeCount; tickCount++)
#endif
	{
		 int amount = 2;
		// Every odd tic...
		if ((tickCount & 1) == 0)
		{
			amount--;
		}
		
		if (ob->velY < 0 && ob->velY >= -amount)
		{
			ck_nextY += ob->velY;
			ob->velY = 0;
			return;
		}
		ob->velY += amount;
		if (ob->velY > MAXFALLSPEED)
		{
			ob->velY = MAXFALLSPEED;
		}
		ck_nextY += ob->velY;
	}
}

//#if 0
/*
====================
=
= DoTinyGravity
=
====================
*/

void FF_DoTinyGravity(CK_object *ob)
{
	// K1n9_Duk3 says:
	// We can't use the optimized loop version here, since that would change the
	// behavior of this routine. The bugged "if (!tickCount)" check uses all 32
	// bits of the long value. But since this is effectively a zero gravity
	// function, we could just omit this and use "ytry += ob->velY * tics;"
	// instead of "DoTinyGravity(ob);".
	long tickCount;
	long lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
	{
		// K1n9_Duk3 says:
		// The following condition is bugged. What I think happened here is
		// that the original code used the condition "if (!tickCount & 3)",
		// which was probably supposed to mean "if ((tickCount & 3) == 0)",
		// but instead turned into "if ((tickCount == 0) & 3)" because the
		// boolean not operator "!" has a higher priority than the bitwise
		// and operator "&" in (Borland's version of) the C language.
		//
		// The result is that instead of increasing the falling speed every
		// 4 game tics (17.5 times per second or roughly every 57 millisecs),
		// the falling speed only increases once every 4 billion tics (once in
		// 710 days of non-stop playing). Which means this is basically a zero
		// gravity function.
		//
		// Fixing this bug is NOT recommended, as it would alter the maximum
		// jump height of Keen's pogo stick, which could make certain parts
		// of some levels unreachable.

		if ((tickCount?0:1) & 3)	// BUG: this effectively means "(if (tickCount == 0))"
		{
			ob->velY += 1;
			if (ob->velY > MAXFALLSPEED)
			{
				ob->velY = MAXFALLSPEED;
			}
		}
		ck_nextY += ob->velY;
	}
}
//#endif

/*
====================
=
= FrictionX
=
====================
*/

void FF_FrictionX(CK_object *ob)
{
	long tickCount, lastTimeCount;
	bool movingLeft = (ob->velX < 0);
	int xAdj;
	if (ob->velX > 0)
		xAdj = -1;
	else if (ob->velX < 0)
		xAdj = 1;
	else
		xAdj = 0;

	lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			ob->velX += xAdj;
			if ((ob->velX < 0) != movingLeft)
			{
				ob->velX = 0;
			}
		}
		ck_nextX += ob->velX;
	}
}

/*
====================
=
= FrictionX2		(K1n9_Duk3 addition)
=
= Applies twice the amount of FrictionX
=
====================
*/

void FF_FrictionX2(CK_object *ob)
{
	int i = SD_GetSpriteSync();
	bool movingLeft = (ob->velX < 0);
	int xAdj;

	if (ob->velX > 0)
		xAdj = -1;
	else if (ob->velX < 0)
		xAdj = 1;
	else
		xAdj = 0;

	while (i--)
	{
		ob->velX += xAdj;
		if ((ob->velX < 0) != movingLeft)
		{
			ob->velX = 0;
		}
		ck_nextX += ob->velX;
	}
}

/*
====================
=
= AccelerateX
=
====================
*/

void FF_AccelerateX(CK_object *ob, int accX, int velLimit)
{
	long tickCount;
	bool isNegative = (ob->velX < 0);
	long lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			ob->velX += accX;
			if ((ob->velX < 0) != isNegative)
			{
				isNegative = (ob->velX < 0);
				ob->xDirection = isNegative ? -1 : 1;
			}

			if (accX > 0 && ob->velX > velLimit)	// K1n9_Duk3 fix: added accX check
			{
				ob->velX = velLimit;
			}
			else if (accX < 0 && ob->velX < -velLimit)	// K1n9_Duk3 fix: added accX check
			{
				ob->velX = -velLimit;
			}
		}
		ck_nextX += ob->velX;
	}
}

/*
====================
=
= FrictionY
=
====================
*/

void FF_FrictionY(CK_object *ob)
{
	long tickCount, lastTimeCount;
	bool movingLeft = (ob->velY < 0);
	int yAdj;
	if (ob->velY > 0)
		yAdj = -1;
	else if (ob->velY < 0)
		yAdj = 1;
	else
		yAdj = 0;

	lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			ob->velY += yAdj;
			if ((ob->velY < 0) != movingLeft)
			{
				ob->velY = 0;
			}
		}
		ck_nextY += ob->velY;
	}
}

/*
====================
=
= AccelerateY
=
====================
*/

void FF_AccelerateY(CK_object *ob, int accY, int velLimit)
{
	long tickCount;
	bool isNegative = (ob->velY < 0);
	long lastTimeCount = SD_GetLastTimeCount();
	for (tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			ob->velY += accY;
			if ((ob->velY < 0) != isNegative)
			{
				isNegative = (ob->velY < 0);
				ob->yDirection = isNegative ? -1 : 1;
			}

			if (accY > 0 && ob->velY > velLimit)	// K1n9_Duk3 fix: added accY check
			{
				ob->velY = velLimit;
			}
			else if (accY < 0 && ob->velY < -velLimit)	// K1n9_Duk3 fix: added accY check
			{
				ob->velY = -velLimit;
			}
		}
		ck_nextY += ob->velY;
	}
}
