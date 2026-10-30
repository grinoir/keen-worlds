/* Keen Dreams Gold Edition: Keen Worlds native companion
 * Copyright (C) 2026 Anders Christensen
 *
 * The SDL2 port's presentation layer (id_vw_gl.c, by David Gow) redone with
 * SDL's own renderer, so no OpenGL or GLEW is needed. Same VW_GL_* calls.
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

#include "id_heads.h"
#include <SDL.h>

unsigned lastdrawnbuffer;
unsigned lastdrawnpan;
extern SDL_Window *window;
extern const uint8_t VW_EGAPalette[16][3];

static SDL_Renderer *vw_renderer;
static SDL_Texture *vw_texture;
static uint32_t vw_palette[16];
static uint32_t vw_pixels[320*200];

void VW_GL_SetVGAPalette(const uint8_t vgapal[16][3])
{
	for (int i = 0; i < 16; ++i)
		vw_palette[i] = 0xff000000u | (vgapal[i][0] << 16) | (vgapal[i][1] << 8) | vgapal[i][2];
}

void VW_GL_SetEGAPalette(char egapal[17])
{
	for (int i = 0; i < 16; ++i)
	{
		int index = ((egapal[i] >> 1) & 8) | (egapal[i] & 7);
		VW_CurrentRGBPalette[i][0] = VW_EGAPalette[index][0];
		VW_CurrentRGBPalette[i][1] = VW_EGAPalette[index][1];
		VW_CurrentRGBPalette[i][2] = VW_EGAPalette[index][2];
	}
	VW_GL_SetVGAPalette(VW_CurrentRGBPalette);
}

void VW_GL_SetIcon(SDL_Window *wnd)
{
	(void)wnd;
}

void VW_GL_Init()
{
	vw_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!vw_renderer)
		vw_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!vw_renderer)
		Quit("Could not create a renderer for the window!");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	vw_texture = SDL_CreateTexture(vw_renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, 320, 200);
	if (!vw_texture)
		Quit("Could not create the screen texture!");
	VW_GL_SetVGAPalette(VW_EGAPalette);
}

void VW_GL_GetViewport(int *x, int *y, int *w, int *h)
{
	int winw, winh;
	if (vw_renderer)
		SDL_GetRendererOutputSize(vw_renderer, &winw, &winh);
	else
		SDL_GetWindowSize(window, &winw, &winh);
	if (aspectcorrect)
	{
		int neww = winh * 4/3;
		if (neww <= winw)
		{
			if (x) *x = (winw - neww) / 2;
			if (y) *y = 0;
			if (w) *w = neww;
			if (h) *h = winh;
		}
		else
		{
			int newh = winw * 3 / 4;
			if (x) *x = 0;
			if (y) *y = (winh - newh) / 2;
			if (w) *w = winw;
			if (h) *h = newh;
		}
	}
	else
	{
		if (x) *x = 0;
		if (y) *y = 0;
		if (w) *w = winw;
		if (h) *h = winh;
	}
}

void VW_GL_SetFullscreen(boolean fullscreen)
{
	SDL_SetWindowFullscreen(window, fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0);
}

boolean VW_GL_IsFullscreen()
{
	return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
}

void VW_GL_UpdateLineWidth()
{
}

void VW_GL_UpdateGLBuffer()
{
	// Convert the visible 320x200 from emulated video memory, following
	// the CRTC start and pel pan; video memory wraps around.
	unsigned start = lastdrawnbuffer + lastdrawnpan;
	for (int y = 0; y < 200; ++y)
	{
		unsigned row = start + y * linewidth;
		uint32_t *dst = &vw_pixels[y * 320];
		for (int x = 0; x < 320; ++x)
			dst[x] = vw_palette[vw_videomem[(row + x) % VW_VIDEOMEM_SIZE] & 15];
	}
}

void VW_GL_Present()
{
	int vx, vy, vw, vh;
	SDL_Rect dst;

	if (!vw_renderer)
		return;
	SDL_UpdateTexture(vw_texture, NULL, vw_pixels, 320 * sizeof(uint32_t));
	SDL_SetRenderDrawColor(vw_renderer, 0, 0, 0, 255);
	SDL_RenderClear(vw_renderer);
	VW_GL_GetViewport(&vx, &vy, &vw, &vh);
	dst.x = vx; dst.y = vy; dst.w = vw; dst.h = vh;
	SDL_RenderCopy(vw_renderer, vw_texture, NULL, &dst);

	// Without vsync, limit us to ~70 Hz.
	static Uint32 lastswap = 0;
	Uint32 frametime = SDL_GetTicks() - lastswap;
	if (frametime < (1000/70))
		SDL_Delay(1000/70 - frametime);
	SDL_RenderPresent(vw_renderer);
	lastswap = SDL_GetTicks();
}
