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
#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "config.h"
#include "resource.h"
#include "sutils.h"

//#define UTILS_USE_SDL_IMAGE
extern SDL_Surface *Screen;
extern ResourceManager *Data;
//-----------------------------------------------------------------------------
//! exception class
Exiter::Exiter(std::string m)
{
    message = m;
};
//-----------------------------------------------------------------------------
// Return the pixel value at (x, y)
// NOTE: The surface must be locked before calling this!
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
        case 1:         return *p;
        case 2:         return *(Uint16 *)p;
        case 3:
            #pragma warn -ccc
            #pragma warn -rch
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            #pragma warn +ccc
            #pragma warn +rch

        case 4:         return *(Uint32 *)p;
        default:        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
//-----------------------------------------------------------------------------
// Set the pixel at (x, y) to the given value
// NOTE: The surface must be locked before calling this!
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
        case 1:        *p = pixel;                  break;
        case 2:        *(Uint16 *)p = pixel;        break;
        case 3:
            #pragma warn -ccc
            #pragma warn -rch
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
            #pragma warn +ccc
            #pragma warn +rch

        case 4:         *(Uint32 *)p = pixel;       break;
    }
}
//-----------------------------------------------------------------------------
bool LoadImage(SDL_Surface **surface, const char *filename)
{
    SDL_Surface *temp;
    printf("Loading image: %s ...", filename);
    #ifdef UTILS_USE_SDL_IMAGE
    temp = IMG_Load(filename);
    #else
    temp = SDL_LoadBMP(filename);
    #endif
    if (!temp)
    {
        printf("FAILED.\nError: %s\n", SDL_GetError());
        return false;
    }
    printf("OK.\n");

    // Convert image to video format
    *surface = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    if ( *surface == NULL )
    {
        printf("Couldn't convert image: %s\n",  SDL_GetError());
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
void Gradient(SDL_Surface *surface, SDL_Rect& r, Uint8 *from, Uint8 *to)
{
    if (r.h == 0)
        return;

    SDL_Rect c;
    double inc[3];
    double color[3];
    for (int i=0; i<3; i++)
    {
        color[i] = from[i];
        inc[i] = ((double)(to[i]) - (double)(from[i])) / (double)(r.h);
    }
    for (int y = 0; y < r.h; y++)
    {
        NjamSetRect(c, r.x, r.y+y, r.w, 1);
        Uint32 rgb = SDL_MapRGB(surface->format,
            (Uint8)color[0], (Uint8)color[1], (Uint8)color[2]);
        SDL_FillRect(surface, &c, rgb);
        for (int i=0; i<3; i++)
            color[i] += inc[i];
    }
}
//-----------------------------------------------------------------------------
bool RGB2OneColor(SDL_Surface *surface, Uint8 r, Uint8 g, Uint8 b)
{
    int x, y;
    Uint32 color = SDL_MapRGB(surface->format, r, g, b);

    if ( SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) < 0 )
    {
        fprintf(stderr, "sutils.cpp: Can't lock surface: %s\n", SDL_GetError());
        return false;
    }

    Uint8 r1, g1, b1;
    for (int x = 0; x < surface->w; x++)
    {
        for (int y = 0; y < surface->h; y++)
        {
            SDL_GetRGB(getpixel(surface, x, y), surface->format, &r1, &g1, &b1);
            if (r1+g1+b1 > 50)
                putpixel(surface, x, y, color);
        }
    }

    if ( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);

    return true;
}
//-----------------------------------------------------------------------------
void PatternFill(SDL_Rect& rect, SDL_Surface *pattern, SDL_Surface *surface)
{
    int w2 = rect.w;
    int h2 = rect.h;
    int w1 = pattern->w;
    int h1 = pattern->h;

    SDL_Rect to;
    for (int x=0; x < w2/w1 + 1; x++)
    {
        for (int y=0; y < h2/h1 + 1; y++)
        {
            NjamSetRect(to, rect.x + x * w1, rect.y + y * h1);
            SDL_BlitSurface(pattern, NULL, surface, &to);
        }
    }
}
//-----------------------------------------------------------------------------
void NjamSetRect(SDL_Rect& rect, int x, int y, int w, int h)
{
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}
//-----------------------------------------------------------------------------
SDLKey NjamGetch(bool Wait)
{
    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
                return event.key.keysym.sym;
            if (event.type == SDL_QUIT)
                throw Exiter("Close button of window clicked");
            if (event.type == SDL_JOYAXISMOTION)
            {
                //printf("Value = %d, Axis = %d\n", event.jaxis.value, event.jaxis.axis);
                const int sensitivity = 12000;
                if (event.jaxis.value < -sensitivity && event.jaxis.axis == 0)
                    return (SDLKey)(500 + event.jaxis.which * 50);
                if (event.jaxis.value >  sensitivity && event.jaxis.axis == 0)
                    return (SDLKey)(501 + event.jaxis.which * 50);
                if (event.jaxis.value < -sensitivity && event.jaxis.axis == 1)
                    return (SDLKey)(502 + event.jaxis.which * 50);
                if (event.jaxis.value >  sensitivity && event.jaxis.axis == 1)
                    return (SDLKey)(503 + event.jaxis.which * 50);
            }
            if (event.type == SDL_JOYBUTTONDOWN)
                return (SDLKey)(504 + event.jbutton.which*50 + event.jbutton.button);
        }
        if (!Wait)
            return SDLK_LAST;
        SDL_Delay(1);
    }
}
//-----------------------------------------------------------------------------
int NjamRandom(int MaxValue)
{
    return ((int)((double)MaxValue*rand()/(RAND_MAX+1.0))); // according to rand man page
}
//-----------------------------------------------------------------------------
bool SurfaceEffect(SDL_Surface *surface, SDL_Rect& r, tEffect Effect)
{
    int x, y;
    if ( SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) < 0 )
    {
        fprintf(stderr, "njamutils.cpp: Can't lock surface: %s\n", SDL_GetError());
        return false;
    }

    Uint8 r1, g1, b1;
    for (int x = r.x; x < r.w+r.x; x++)
    {
        for (int y = r.y; y < r.h+r.y; y++)
        {
            SDL_GetRGB(getpixel(surface, x, y), surface->format, &r1, &g1, &b1);
            int val = (r1 + g1 + b1) / 3;
            Uint32 color;

            if (Effect == fxBlackWhite)
                color = SDL_MapRGB(surface->format, (unsigned char)(val/1.1), (unsigned char)val, (unsigned char)(val/1.1));
            else if (Effect == fxDarken)
                color = SDL_MapRGB(surface->format, r1/2, g1/2, b1/2);
            else if (Effect == fxDarkenAlot)
                color = SDL_MapRGB(surface->format, r1/5, g1/5, b1/5);
            putpixel(surface, x, y, color);
        }
    }

    if ( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);

    return true;
}
//-----------------------------------------------------------------------------
void Box(SDL_Surface *surface, SDL_Rect& rect, int border, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Rect rc;
    NjamSetRect(rc, rect.x, rect.y, rect.w, border);    // upper
    SDL_FillRect(surface, &rc, SDL_MapRGB(surface->format, r, g, b));

    NjamSetRect(rc, rect.x, rect.y+rect.h-border, rect.w, border);  // lower
    SDL_FillRect(surface, &rc, SDL_MapRGB(surface->format, r, g, b));

    NjamSetRect(rc, rect.x, rect.y, border, rect.h);    // left
    SDL_FillRect(surface, &rc, SDL_MapRGB(surface->format, r, g, b));

    NjamSetRect(rc, rect.x+rect.w-border, rect.y, border, rect.h);  // right
    SDL_FillRect(surface, &rc, SDL_MapRGB(surface->format, r, g, b));
}
//-----------------------------------------------------------------------------
void PlaySound(const char *name)
{
    Mix_Chunk *snd = Data->getSfx(name);
    if (!snd)
        return;
    Mix_PlayChannel(-1, snd, 0);
}
//---------------------------------------------------------------------------
int SoundVolume(int change)
{
    int vol = 90;
    config().getValue("soundVolume", vol);
    vol += change;
    if (vol > 100)
        vol = 100;
    if (vol < 0)
        vol = 100;
    if (change != 0)
        config().setValue("soundVolume", vol);

    // play sound to test
    Data->SetSfxVolume(vol);
    Mix_Chunk *c = Data->getSfx("drop");
    if (change != 0 && c)
        Mix_PlayChannel(-1, c, 0);
    return vol;
}
//---------------------------------------------------------------------------
int MusicVolume(int change)
{
    int vol = 90;
    config().getValue("musicVolume", vol);
    vol += change;
    if (vol > 100)
        vol = 100;
    if (vol < 0)
        vol = 100;
    if (change != 0)
        config().setValue("musicVolume", vol);

    Mix_VolumeMusic(vol);
    return vol;
}
//-----------------------------------------------------------------------------
void CheckMusic(Mix_Music *m)
{
    static Mix_Music* tune = 0;
    if (m == 0)
        m = tune;
    if (m != tune)
    {
        if (Mix_PlayingMusic())
            Mix_HaltMusic();
        tune = m;
    }

    if (tune && !Mix_PlayingMusic())
    {
        if (Mix_PlayMusic(tune, 1) == -1)
            fprintf(stderr, "ERROR: Mix_PlayMusic: %s\n", Mix_GetError());
        else
            MusicVolume(0);     // reset to current
    }
}
//-----------------------------------------------------------------------------
void InitVideo(bool fullscreen)
{
    SDL_WM_SetCaption("Vodovod " PROG_VERSION, NULL);

    Uint32 flags = SDL_ANYFORMAT|SDL_SWSURFACE;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;

    printf("Setting video mode...\n");
    int modes[] = { 16, 24, 32, 8 };
    for (int i=0; i<sizeof(modes)/sizeof(int); ++i)
    {
        fprintf(stderr, "Trying video mode 800x600x%d...", modes[i]);
        Screen = SDL_SetVideoMode(800, 600, modes[i], flags);
        if (Screen)
            break;
        fprintf(stderr, "failed: %s\n", SDL_GetError());
    }
    if (!Screen)
        throw Exiter(SDL_GetError());
    printf("Ok.\n");

    SDL_Surface *icon = Data->getGfx("icon", "data/abicon.bmp");
    if (!icon)
        throw Exiter(SDL_GetError());
    SDL_WM_SetIcon(icon, NULL);

    // hide mouse cursor in fullscreen
    if (fullscreen)
        SDL_ShowCursor(SDL_DISABLE);
    else
        SDL_ShowCursor(SDL_ENABLE);
}
//-----------------------------------------------------------------------------
