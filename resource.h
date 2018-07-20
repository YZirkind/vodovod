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
#ifndef MB_RESOURCE_H
#define MB_RESOURCE_H

#include <string>
#include <map>
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
//-----------------------------------------------------------------------------
// resource manager
class ResourceManager
{
public:
    ~ResourceManager();
    SDL_Surface *getGfx(const char *alias, const char *filename = 0);
    Mix_Music *getMusic(const char *alias, const char *filename = 0);
    Mix_Chunk *getSfx(const char *alias, const char *filename = 0);
    void SetSfxVolume(int volume);

private:
    typedef std::map<std::string, SDL_Surface*> GfxContainer;
    GfxContainer graphicsM;
    typedef std::map<std::string, Mix_Music*> MusicContainer;
    MusicContainer musicM;
    typedef std::map<std::string, Mix_Chunk*> SfxContainer;
    SfxContainer soundsM;
};
//-----------------------------------------------------------------------------
#endif
