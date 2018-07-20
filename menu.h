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
#ifndef AB_MENU_H
#define AB_MENU_H
#include <string>
#include <vector>
#include "sutils.h"
#include "njamfont.h"
//-----------------------------------------------------------------------------
class Menu
{
protected:
    bool firstIsTitleM;
    NjamFont *font;
    SDL_Surface *background;
    int indicator;                      // index of indicator
    int x_pos, y_pos;                   // position on screen, set by caller
    bool process_keys();                // up/down/enter/esc: return false on menu exit
    virtual void draw(bool flipit = true);
    virtual void render() {};
    virtual bool onEnter() = 0;
    void getDimensions(SDL_Rect& rect, int border, int minWidth);
    SDLKey Message(std::string text);   // returns the key pressed

public:
    Menu(NjamFont *font_ptr, int x, int y);
    virtual ~Menu();
    std::vector<std::string> options;
    void start(int minWidth = 0);

};
//-----------------------------------------------------------------------------
#endif
