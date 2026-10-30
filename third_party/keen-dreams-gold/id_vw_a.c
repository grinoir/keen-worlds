/* Keen Dreams (SDL2/Steam Port) Source Code
 * Copyright (C) 2014 David Gow <david@davidgow.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Stubs for id_vw_a.asm

#include "id_heads.h"


void VW_WaitVBL(int vblanks)
{
	while (vblanks--)
	{
		IN_PumpEvents();
		VW_GL_Present();
	}
}

cardtype VW_VideoID()
{
	return VGAcard;
}

boolean FindPS2()
{
	return false;
}

boolean FindEGA()
{
	return true;
}

boolean FindCGA()
{
	return true;
}

boolean FindMono()
{
	return true;
}

boolean Find6845()
{
	return true;
}

void FindActive()
{
}

void FoundDevice()
{
}
