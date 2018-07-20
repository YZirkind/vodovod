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
#ifndef AB_ALLMENUS_H
#define AB_ALLMENUS_H
//-----------------------------------------------------------------------------
#include <list>
#include "effects.h"
#include "menu.h"
#include "njamfont.h"
//-----------------------------------------------------------------------------
class MainMenu: public Menu
{
private:
    Effects effectsM;
    NjamFont whiteFontM;

    // create/destroy and stuff
    std::vector<SDL_Joystick *> joysM;

    void start_game(int players);
    virtual bool onEnter();
    virtual void render();
public:
    MainMenu(NjamFont *font_ptr, int x, int y);
    ~MainMenu();
};
//-----------------------------------------------------------------------------
class OptionsMenu: public Menu
{
public:
    OptionsMenu(NjamFont *font_ptr, int x, int y);
    virtual bool onEnter();
};
//-----------------------------------------------------------------------------
class KeyboardMenu: public Menu
{
public:
    KeyboardMenu(NjamFont *font_ptr, int x, int y);
    virtual bool onEnter();
};
//-----------------------------------------------------------------------------
class SelectGameTypeMenu: public Menu
{
public:
    SelectGameTypeMenu(NjamFont *font_ptr, int x, int y);
    virtual bool onEnter();
};
//-----------------------------------------------------------------------------
#endif
