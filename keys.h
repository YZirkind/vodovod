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
#ifndef AB_KEYS_H
#define AB_KEYS_H

#include <string>
#include <map>
#include "SDL.h"
//-----------------------------------------------------------------------------
class Keyboard
{
private:
    std::map<SDLKey, char> keysM;
public:
    void setKeyDown(SDLKey k, bool state);
    bool isKeyDown(SDLKey k);
    SDLKey update(bool updateRepeated);
};
//-----------------------------------------------------------------------------
class KeyNames
{
private:
    KeyNames();
    std::map<int, std::string> keynames;
public:
    std::string getKeyName(const int& key);
    static KeyNames &keyNamesProvider();
};
//-----------------------------------------------------------------------------
#endif
