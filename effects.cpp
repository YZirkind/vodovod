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
#include <SDL.h>
#include <vector>
#include <list>
#include <algorithm>
#include "njamfont.h"
#include "effects.h"

extern SDL_Surface *Screen;
//-----------------------------------------------------------------------------
Effects::~Effects()
{
    for (std::list<SpecialEffect *>::iterator it = listM.begin(); it != listM.end(); ++it)
        delete (*it);
    listM.clear();
}
//-----------------------------------------------------------------------------
void Effects::addEffect(SpecialEffect *s)
{
    listM.push_back(s);
}
//-----------------------------------------------------------------------------
void Effects::removeEffect(SpecialEffect *s)
{
    for (std::list<SpecialEffect *>::iterator it = listM.begin(); it != listM.end(); ++it)
    {
        SpecialEffect *p = (*it);
        if (p == s)
        {
            listM.erase(it);
            delete p;
            return;
        }
    }
}
//-----------------------------------------------------------------------------
void Effects::Render()
{
    std::vector<SpecialEffect *> to_delete;
    for (std::list<SpecialEffect *>::iterator it = listM.begin(); it != listM.end(); ++it)
    {
        SpecialEffect *p = (*it);
        if (!p->Update())
        {
            to_delete.push_back(p);
            delete p;
        }
    }

    while (!to_delete.empty())
    {
        listM.erase(std::find(listM.begin(), listM.end(), to_delete.back()));
        to_delete.pop_back();
    }
}
//-----------------------------------------------------------------------------
// VARIOUS EFFECTS BEGIN HERE
//-----------------------------------------------------------------------------
bool ScoreEffect::Update()
{
    counterM++;             // 1 - 100
    if (counterM > 60)
        return false;

    char buffer[30];
    sprintf(buffer, "%d", pointsM);
    fontM->setAlpha(255 - counterM*3);
    fontM->WriteTextXY(Screen, xposM + counterM/2, yposM - counterM/2, buffer);
    fontM->setAlpha(255);
    return true;
}
//-----------------------------------------------------------------------------
bool TextEffect::Update()
{
    counterM++;             // 1 - 100
    if (counterM > 100)
    {
        if (repeatM)
        {
            if (repeatM > 0)
                repeatM--;
            counterM = 1;
        }
        else
            return false;
    }
    if (counterM < 30)
        fontM->setAlpha((Uint8)(counterM*8.5));
    else
        fontM->setAlpha((Uint8)(255.0 - 3.6*(counterM-30.0)));
    fontM->WriteTextXY(Screen, xposM, yposM, textM.c_str());
    fontM->setAlpha(255);
    return true;
}
//-----------------------------------------------------------------------------
//   1 -  60 scroll in
//  61 - 150 display
// 151 - 210 scroll out
bool ScrollEffect::Update()
{
    counterM++;             // 1 -
    if (counterM > 260)
        return false;

    if (counterM < 61)
        xoffset -= ((800.0-(double)xposM)/60.0);
    else if (counterM > 149)
        xoffset -= (double)(textM.length()*fontM->GetCharWidth()+xposM) / 60.0;

    fontM->WriteTextXY(Screen, (int)xoffset, (int)yposM, textM.c_str());
    return true;
}
//-----------------------------------------------------------------------------
