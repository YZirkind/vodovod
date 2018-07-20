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
#ifndef AB_EFFECTS_H
#define AB_EFFECTS_H

#include <list>
#include <string>
//-----------------------------------------------------------------------------
class Game;
//-----------------------------------------------------------------------------
class SpecialEffect
{
protected:
    NjamFont *fontM;

public:
    SpecialEffect(NjamFont *f) { fontM = f; };
    virtual bool Update() = 0;
};
//-----------------------------------------------------------------------------
class Effects
{
private:
    std::list<SpecialEffect *> listM;
public:
    void addEffect(SpecialEffect *);
    void removeEffect(SpecialEffect *);
    void Render();

    ~Effects();
};
//-----------------------------------------------------------------------------
class ScoreEffect: public SpecialEffect
{
private:
    int counterM, xposM, yposM, pointsM;
public:
    ScoreEffect(NjamFont *f, int xpos, int ypos, int points)
        : SpecialEffect(f), counterM(0), xposM(xpos), yposM(ypos), pointsM(points) {};
    virtual bool Update();
};
//-----------------------------------------------------------------------------
class TextEffect: public SpecialEffect
{
private:
    int repeatM;
    int counterM, xposM, yposM;
    std::string textM;
public:
    TextEffect(NjamFont *f, int xpos, int ypos, const std::string& text, int repeat = 0)
        : SpecialEffect(f), counterM(0), xposM(xpos), yposM(ypos), textM(text), repeatM(repeat) {};
    virtual bool Update();
};
//-----------------------------------------------------------------------------
class ScrollEffect: public SpecialEffect
{
private:
    double xoffset;
    int counterM, xposM, yposM;
    std::string textM;
public:
    ScrollEffect(NjamFont *f, int xpos, int ypos, const std::string& text)
        : SpecialEffect(f), counterM(0), xposM(xpos), yposM(ypos), textM(text), xoffset(800) {};
    virtual bool Update();
};
//-----------------------------------------------------------------------------
#endif


