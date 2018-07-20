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
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "njamfont.h"
#include "sutils.h"

#include "config.h"
#include "menu.h"
#include "allmenus.h"
#include "resource.h"
SDL_Surface *Screen;        // global objects
ResourceManager *Data;
//-----------------------------------------------------------------------------
void RenderBackground(int housex)
{
    SDL_Surface *dirt  = Data->getGfx("dirt");
    SDL_Surface *house = Data->getGfx("house");
    SDL_Surface *tree  = Data->getGfx("tree1");

    SDL_Rect r;
    NjamSetRect(r, 0, 97, 800, 503);
    PatternFill(r, dirt, Screen);
    NjamSetRect(r, 10, 110, 779, 419);
    Box(Screen, r, 1, 30, 30, 30);

    NjamSetRect(r, 0, 0, 800, 94);
    Uint8 from[] = {  40, 183, 208 };
    Uint8 to[]   = { 227, 245, 249 };
    Gradient(Screen, r, from, to);

    NjamSetRect(r, 0, 94, 800, 3);
    Uint8 gfrom[] = { 103, 176,  0 };
    Uint8 gto[]   = {  66, 228, 28 };
    Gradient(Screen, r, gfrom, gto);

    NjamSetRect(r, housex, 55);
    SDL_BlitSurface(house, 0, Screen, &r);

    NjamSetRect(r, housex < 500 ? 700 : 100, 51);
    SDL_BlitSurface(tree, 0, Screen, &r);
}
//-----------------------------------------------------------------------------
bool InitAudio()
{
    printf("Opening audio.\n");
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 2048) == -1)
    {
        fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
        return false;
    }
    else
    {
        fprintf(stderr, "Reserving channels for sfx...");
        fprintf(stderr, "(%d of 4).\n", Mix_ReserveChannels(4));
        return true;
    }
}
//-----------------------------------------------------------------------------
//! initialize SDL, and start main menu
int main(int /* argc */, char ** /* argv */)
{
    Screen = 0;

    printf("Initializing SDL: VIDEO, AUDIO & JOYSTICK...");
    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK ) < 0 )
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    printf("Ok.\n");

    bool audio = false;
    try
    {
        ResourceManager res;
        Data = &res;

        bool Fullscreen = false;
        config().getValue("fullscreen", Fullscreen);
        InitVideo(Fullscreen);
        audio = InitAudio();

        SDL_Surface *vodovod = Data->getGfx("vodovod", "data/vodovod.png");
        Data->getGfx("dirt", "data/slate.png");
        Data->getGfx("queue", "data/queue.png");
        Data->getGfx("blocks", "data/block.png");

        // house
        SDL_Surface *house = Data->getGfx("house", "data/house.png");
        Uint32 green = SDL_MapRGB(house->format, 0, 255, 0);
        SDL_SetColorKey(house, SDL_SRCCOLORKEY, green);

        // tree
        SDL_Surface *tree = Data->getGfx("tree1", "data/tree1.png");
        Uint32 red = SDL_MapRGB(tree->format, 255, 0, 0);
        SDL_SetColorKey(tree, SDL_SRCCOLORKEY, red);

        // human walking
        SDL_Surface *human = Data->getGfx("human", "data/walk.png");
        Uint32 back = SDL_MapRGB(human->format, 223, 232, 255);
        SDL_SetColorKey(human, SDL_SRCCOLORKEY, back);

        // preload music
		if (audio)
		{
			Data->getMusic("walk", "data/walk.xm");
			Data->getMusic("flow", "data/flow.xm");
			Data->getMusic("tune", "data/vodovod.xm");

			// preload sounds
			Data->getSfx("drop", "data/drop.wav");
			Data->getSfx("levelend", "data/mapend.wav");
			Data->getSfx("bubbles", "data/bubbles.wav");
			Data->getSfx("metal", "data/metal.wav");
		}

        // render some background picture for menu
        RenderBackground(630);

        SDL_Rect r;
        NjamSetRect(r, 50, 150);
        SDL_BlitSurface(vodovod, 0, Screen, &r);
        SDL_Flip(Screen);

        NjamFont font("data/font-yellow.bmp", 11, 22);
        MainMenu m(&font, 95, 280);
        m.start();
    }
    catch(Exiter &e)
    {
        printf("\nExiting: %s\n", e.message.c_str());
    }

    if (audio)
        SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
//-----------------------------------------------------------------------------
