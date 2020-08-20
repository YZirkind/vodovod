/*-----------------------------------------------------------------------------
Copyright 2007 Milan Babuskov

This file is part of Vodovod

Vodovod is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Vodovod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Vodovod in file COPYING; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-----------------------------------------------------------------------------*/
#ifndef SUTILS_H
#define SUTILS_H
#include <string>
#include <map>
#include "SDL.h"
#include "SDL_mixer.h"

#define PROG_VERSION "1.10"
//-----------------------------------------------------------------------------
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
bool LoadImage(SDL_Surface **surface, const char *filename);
bool RGB2OneColor(SDL_Surface *surface, Uint8 r, Uint8 g, Uint8 b);
void PatternFill(SDL_Rect& rect, SDL_Surface *pattern, SDL_Surface *surface);
void SDL_Rect_set(SDL_Rect& rect, int x, int y, int w=0, int h=0);


void Gradient(SDL_Surface *surface, SDL_Rect& r, Uint8 *from, Uint8 *to);
SDLKey NjamGetch(bool Wait);
int NjamRandom(int MaxValue);
void Box(SDL_Surface *surface, SDL_Rect& rect, int border, Uint8 r, Uint8 g, Uint8 b);

void InitVideo(bool fullscreen);
void PlaySound(const char *name);
int MusicVolume(int change);
int SoundVolume(int change);
void CheckMusic(Mix_Music *);



typedef enum { fxBlackWhite, fxDarken, fxDarkenAlot } tEffect;
bool SurfaceEffect(SDL_Surface *surface, SDL_Rect& r, tEffect Effect = fxBlackWhite);
//-----------------------------------------------------------------------------
//! exception class
class Exiter
{
public:
    std::string message;
    Exiter(std::string m);
};
//-----------------------------------------------------------------------------
#endif

