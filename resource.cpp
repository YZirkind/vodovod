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
#include "resource.h"
//-----------------------------------------------------------------------------
Mix_Chunk *ResourceManager::getSfx(const char *alias, const char *filename)
{
    SfxContainer::iterator it = soundsM.find(alias);
    if (it != soundsM.end())
        return (*it).second;

    if (filename == 0)  // requested but not loaded
    {
        fprintf(stderr, "Uninitialized sfx: %s\n", alias);
        return 0;
    }

    fprintf(stderr, "Loading sfx %s...", filename);
    Mix_Chunk *temp = Mix_LoadWAV(filename);
    if (!temp)
    {
        fprintf(stderr, "ERROR: Mix_LoadWAV(): %s\n", Mix_GetError());
        return 0;
    }

    soundsM.insert(SfxContainer::value_type(alias, temp));
    fprintf(stderr, "Ok.\n");
    return temp;
}
//-----------------------------------------------------------------------------
Mix_Music *ResourceManager::getMusic(const char *alias, const char *filename)
{
    MusicContainer::iterator it = musicM.find(alias);
    if (it != musicM.end())
        return (*it).second;

    if (filename == 0)  // requested but not loaded
    {
        fprintf(stderr, "Uninitialized music: %s\n", alias);
        return 0;
    }

    fprintf(stderr, "Loading music %s (%s)...", alias, filename);
    Mix_Music *temp = Mix_LoadMUS(filename);
    if (!temp)
    {
        fprintf(stderr, "ERROR: Mix_LoadMUS(): %s\n", Mix_GetError());
        return 0;
    }

    musicM.insert(MusicContainer::value_type(alias, temp));
    fprintf(stderr, "Ok.\n");
    return temp;
}
//-----------------------------------------------------------------------------
SDL_Surface *ResourceManager::getGfx(const char *alias, const char *filename)
{
    GfxContainer::iterator it = graphicsM.find(alias);
    if (it != graphicsM.end())
        return (*it).second;

    if (filename == 0)  // requested but not loaded
    {
        fprintf(stderr, "Uninitialized graphics: %s\n", alias);
        return 0;
    }

    fprintf(stderr, "Loading graphics %s (%s)...", alias, filename);
    SDL_Surface *temp = IMG_Load(filename);
    if (!temp)
    {
        fprintf(stderr, " failed. Error: %s\n", SDL_GetError());
        return 0;
    }
    fprintf(stderr, "OK. Converting...");

    // Convert image to video format
    SDL_Surface *temp2 = SDL_DisplayFormat(temp);
    if (!temp2)
    {
        fprintf(stderr, "failed: %s\n", SDL_GetError());
        temp2 = temp;
    }
    else
        SDL_FreeSurface(temp);

    graphicsM.insert(GfxContainer::value_type(alias, temp2));
    fprintf(stderr, "Ok.\n");
    return temp2;
}
//-----------------------------------------------------------------------------
void ResourceManager::SetSfxVolume(int volume)
{
    for (SfxContainer::iterator it = soundsM.begin(); it != soundsM.end();
        ++it)
    {
        Mix_VolumeChunk((*it).second, volume);
    }
}
//-----------------------------------------------------------------------------
// THIS MUST!!! be called before SDL_Quit and SDL_CloseAudio
ResourceManager::~ResourceManager()
{
    fprintf(stderr, "Releasing resources...\n");
    for (GfxContainer::iterator it = graphicsM.begin(); it != graphicsM.end(); ++it)
    {
        fprintf(stderr, "Freeing gfx resource: %s...", (*it).first.c_str());
        SDL_FreeSurface((*it).second);
        fprintf(stderr, "OK.\n");
    }
    for (SfxContainer::iterator it = soundsM.begin(); it != soundsM.end(); ++it)
    {
        fprintf(stderr, "Freeing sfx resource: %s...", (*it).first.c_str());
        Mix_FreeChunk((*it).second);
        fprintf(stderr, "OK.\n");
    }
    for (MusicContainer::iterator it = musicM.begin(); it != musicM.end(); ++it)
    {
        fprintf(stderr, "Freeing music resource: %s (0x%08x)...", (*it).first.c_str(), (*it).second);
        Mix_FreeMusic((*it).second);
        fprintf(stderr, "OK.\n");
    }
}
//-----------------------------------------------------------------------------
