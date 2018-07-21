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
#include "SDL.h"
#include "config.h"
#include "sutils.h"
#include "resource.h"
#include "map.h"

extern SDL_Surface* Screen;
extern ResourceManager *Data;
//-----------------------------------------------------------------------------
Shape::Shape()
    :fillM(0), flowFromM(dNone), flowToM(dNone)
{
}
//-----------------------------------------------------------------------------
int Shape::GetSpeed()
{
    return 60;
}
//-----------------------------------------------------------------------------
int SlowShape::GetSpeed()
{
    return 180;
}
//-----------------------------------------------------------------------------
int Shape::GetPoints()
{
    return 0;
}
//-----------------------------------------------------------------------------
Direction Shape::LeaksTo()
{
    return flowToM;
}
//-----------------------------------------------------------------------------
bool Shape::IsDestroyable()
{
    return (fillM == 0);
}
//-----------------------------------------------------------------------------
// returns true if full
bool Shape::SetFill(int fill, Direction flowFrom)
{
    flowFromM = flowFrom;
    int speed = GetSpeed();
    if (fill >= speed)      // end of
    {
        fillM = speed;
        return true;
    }
    fillM = fill;
    return false;
}
//-----------------------------------------------------------------------------
int RegularShape::GetPoints()
{
    int sum = 1;
    int reduce = 0;
    for (int i=0; i<4; i++)
    {
        if (directionsM[i] == dFull)
            sum += 2;
        if (directionsM[i] == i)
            reduce = 1;
    }
    return (sum-reduce) * 10;
}
//-----------------------------------------------------------------------------
bool RegularShape::SetFill(int fill, Direction flowFrom)
{
    if (fill >= GetSpeed())
        directionsM[flowToM] = dFull;  // second half full
    else if (fill >= GetSpeed()/2)   // passed half
    {
        directionsM[flowFrom] = dFull;
        flowToM = (Direction)(3 - flowFrom);
        if (directionsM[flowToM] != flowToM)
        {
            for (int i=0; i<dNone; i++)
            {
                if (directionsM[i] == i)
                {
                    flowToM = (Direction)i;
                    break;
                }
            }
        }
    }
    return Shape::SetFill(fill, flowFrom);
}
//-----------------------------------------------------------------------------
void Shape::Render(int /* x */, int /* y */, int /* scale */)
{
}
//-----------------------------------------------------------------------------
CheckPoint::CheckPoint()
    :RegularShape('-')
{
}
//-----------------------------------------------------------------------------
void CheckPoint::Render(int x, int y, int scale)
{
    SDL_Surface *block = Data->getGfx("blocks");
    SDL_Rect src, dest;
    NjamSetRect(src,  60, 0, 60, 60);
    NjamSetRect(dest, x, y);
    SDL_BlitSurface(block, &src, Screen, &dest);
    RegularShape::Render(x, y, scale);
}
//-----------------------------------------------------------------------------
bool CheckPoint::IsDestroyable()
{
    return false;
}
//-----------------------------------------------------------------------------
int CheckPoint::GetPoints()
{
    return 50;
}
//-----------------------------------------------------------------------------
void RegularShape::Render(int x, int y, int scale)
{
    const int w    =  8 / scale;
    const int cell = 60 / scale;

    Uint32 black = SDL_MapRGB(Screen->format, 0, 0, 0);
    Uint32 water = SDL_MapRGB(Screen->format, 0, 180, 250);
    int lef[] = {          0, (cell-w)/2, (cell-w)/2, (cell-w)/2 };
    int top[] = { (cell-w)/2,          0, (cell-w)/2, (cell-w)/2 };
    int wid[] = { (cell+w)/2,          w,          w, (cell+w)/2 };
    int hei[] = {          w, (cell+w)/2, (cell+w)/2,          w };
    // render cross
    for (int i=0; i<dNone; i++)
    {
        Direction d = directionsM[(Direction)i];
        if (d == i || d == dFull)
        {
            SDL_Rect r;
            NjamSetRect(r, x+lef[i], y+top[i], wid[i], hei[i]);
            SDL_FillRect(Screen, &r, black);
        }
    }
    // render ooze
    int lefo[] = {          0, (cell-w)/2, (cell-w)/2, (cell)/2 };
    int topo[] = { (cell-w)/2,          0, (cell)/2, (cell-w)/2 };
    int wido[] = {   (cell)/2,          w,          w, (cell)/2 };
    int heio[] = {          w, (cell)/2,     (cell)/2,        w };
    for (int i=0; i<dNone; i++)
    {
        Direction d = directionsM[(Direction)i];
        int xi = (i == 0 || i == 3 ? 0 : 1);
        int yi = (i == 0 || i == 3 ? 1 : 0);
        if (d == dFull)
        {
            SDL_Rect r;
            NjamSetRect(r, x+lefo[i]+xi, y+topo[i]+yi, wido[i]-xi*2, heio[i]-yi*2);
            SDL_FillRect(Screen, &r, water);
        }
    }

    if (fillM > 0 && fillM < GetSpeed())
    {
        SDL_Rect r;
        if (fillM < GetSpeed() / 2)
        {
            Direction d = flowFromM;
            int kw = (cell*fillM)/GetSpeed();
            if (d == dLeft)
                NjamSetRect(r, x,          y+top[d]+1, kw, hei[d]-2);
            if (d == dRight)
                NjamSetRect(r, x+cell-kw,  y+top[d]+1, kw, hei[d]-2);
            if (d == dUp)
                NjamSetRect(r, x+lef[d]+1, y,          wid[d]-2, kw);
            if (d == dDown)
                NjamSetRect(r, x+lef[d]+1, y+cell-kw,  wid[d]-2, kw);
        }
        else
        {
            Direction d = flowToM;
            int kw = (cell*(fillM-cell/2))/GetSpeed();
            if (d == dRight)
                NjamSetRect(r, x+cell/2,          y+top[d]+1, kw, hei[d]-2);
            if (d == dLeft)
                NjamSetRect(r, x+cell/2-kw,  y+top[d]+1, kw, hei[d]-2);
            if (d == dDown)
                NjamSetRect(r, x+lef[d]+1, y+cell/2,          wid[d]-2, kw);
            if (d == dUp)
                NjamSetRect(r, x+lef[d]+1, y+cell/2-kw,  wid[d]-2, kw);
        }
        SDL_FillRect(Screen, &r, water);
    }
}
//-----------------------------------------------------------------------------
bool Shape::HasDirection(Direction)
{
    return false;
}
//-----------------------------------------------------------------------------
// All pipe shapes are drawn with arms connecting to the center of the pipe's cell
// to the adjancent cells.
//
// Given our shape character, determine which are arm directions are needed.
RegularShape::RegularShape(char which)
    :Shape()
{
	for (int i=0; i<dNone; i++) {
		directionsM[i] = dNone;
	}

	// +--  --+
	// |      |
	//
	// |      |
	// +--  --+


	// L : Connected Right and Up.  Lower left corner.
	// J : Connected Left and Up.  Lower right corner.
	// F : Connected Right and Down.  Upper left corner.
	// 7 : Connected Left and Down.  Upper right corner.
	// - : Connected Left and Right.  Horizontal pipe.
	// + : Connected in all directions.
	// | : Connected Up and Down.  Vertical pipe.


	if (which == '+' || which == '-' || which == 'J' || which == '7') {
		directionsM[dLeft] = dLeft;
	}

	if (which == '+' || which == '-' || which == 'F' || which == 'L') {
		directionsM[dRight] = dRight;
	}

	if (which == '+' || which == '|' || which == 'J' || which == 'L') {
		directionsM[dUp] = dUp;
	}

	if (which == '+' || which == '|' || which == 'F' || which == '7') {
		directionsM[dDown] = dDown;
	}
}
//-----------------------------------------------------------------------------
bool RegularShape::HasDirection(Direction d)
{
    return (directionsM[d] == d);
}
//-----------------------------------------------------------------------------
int SlowShape::GetPoints()
{
    return 30;
}
//-----------------------------------------------------------------------------
// left to right or right to left only
void SlowShape::Render(int x, int y, int scale)
{
    const int w    =  8 / scale;
    const int cell = 60 / scale;

    Uint32 black = SDL_MapRGB(Screen->format, 0, 0, 0);
    Uint32 water = SDL_MapRGB(Screen->format, 0, 180, 250);

    SDL_Rect r;
    NjamSetRect(r, x,   y+(cell-w)/2, cell,   w*3);
    SDL_FillRect(Screen, &r, black);

    // render ooze
    if (fillM > 0)
    {
        if (flowFromM == dLeft || fillM >= GetSpeed())
        {
            NjamSetRect(r, x, y+(cell-w)/2+1, 1, w-2);
            SDL_FillRect(Screen, &r, water);
        }
        if (flowFromM == dRight || fillM >= GetSpeed())
        {
            NjamSetRect(r, x+cell-1, y+(cell-w)/2+1, 1, w-2);
            SDL_FillRect(Screen, &r, water);
        }

        int len = fillM/3 - 2;
        if (len > 0)
        {
            int startfrom = x+1;
            if (flowFromM == dRight)
                startfrom = x+cell-len-1;
            NjamSetRect(r, startfrom, y+(cell-w)/2+1, len, w*3-2);
            SDL_FillRect(Screen, &r, water);
        }
    }
}
//-----------------------------------------------------------------------------
SlowShape::SlowShape()
    :RegularShape('-')
{
}
//-----------------------------------------------------------------------------
/*
bool SlowShape::HasDirection(Direction d)
{
    return d == dLeft || d == dRight;
}*/
//-----------------------------------------------------------------------------
Obstacle::Obstacle()
    :Shape()
{
    fillM = 1000;   // cannot enter, cannot overwrite
}
//-----------------------------------------------------------------------------
void Obstacle::Render(int x, int y, int /* scale */)
{
    // bones, head, whatever
    SDL_Surface *block = Data->getGfx("blocks");
    if (!block)
        return;
    SDL_Rect src, dest;
    NjamSetRect(src,  0, 0, 60, 60);
    NjamSetRect(dest, x, y);
    SDL_BlitSurface(block, &src, Screen, &dest);
}
//-----------------------------------------------------------------------------
Map::Map(int w, int h, int x, int y)
{
    xoffM = x;
    yoffM = y;
    widthM = w;
    heightM = h;
    fieldsM.reserve(w*h);
    for (int i=0; i<w*h; i++)
        fieldsM.push_back(0);
}
//-----------------------------------------------------------------------------
Map::~Map()
{
    for (int i=0; i<widthM*heightM; i++)
        delete fieldsM[i];
}
//-----------------------------------------------------------------------------
void Map::MoveCursor(int byX, int byY)
{
    if (xCursorM + byX > -1 && xCursorM + byX < widthM)
        xCursorM += byX;
    if (yCursorM + byY > -1 && yCursorM + byY < heightM)
        yCursorM += byY;
}
//-----------------------------------------------------------------------------
void Map::Setup(int level, int flowDelay)
{
    // cleanup previous run
    for (int i=0; i<widthM*heightM; i++)
    {
        delete fieldsM[i];
        fieldsM[i] = 0;
    }

    // set obstacles at random places or whatever
    if (level > 6 && (level%6) == 1)
    {
        for (int i=0; i< widthM-3; ++i)
        {
            int f = i+3*widthM;
            delete (fieldsM[f]);
            fieldsM[f] = new Obstacle();
        }
    }
    else if (level > 6 && (level%6) == 3)
    {
        for (int i=0; i< widthM-3; ++i)
        {
            int f = 3+i+3*widthM;
            delete (fieldsM[f]);
            fieldsM[f] = new Obstacle();
        }
    }
    else if (level > 6 && (level%6) == 5)
    {
        for (int i=0; i< widthM; ++i)
        {
            if (i >= 5 && i <= 7)
                continue;
            int f = i+3*widthM;
            delete (fieldsM[f]);
            fieldsM[f] = new Obstacle();
        }
    }
    else
    {
        int obstacles = (level - 1) * 2;
        if (obstacles > 15)
            obstacles = 15;
        for (int i=0; i<obstacles; i++)
        {
            int x = 3+NjamRandom(widthM-6);
            int y = 1+NjamRandom(heightM-2);
            int f = x+y*widthM;
            delete (fieldsM[f]);
            fieldsM[f] = new Obstacle();
        }
    }

    if (level > 2)
    {
        int holes = (level < 7 ? 2 : 1);
        for (int i=0; i<holes; i++)
        {
            int x = 3+NjamRandom(widthM -6);
            int y = 1+NjamRandom(heightM-2);
            for (int f = -1; f < 2; f++)
            {
                delete fieldsM[x+y*widthM+f];
                fieldsM[x+y*widthM+f] = 0;
            }
            fieldsM[x+y*widthM] = new SlowShape();
        }
    }

    if (level > 3)
    {
        fieldsM[1+widthM*(1+NjamRandom(2))] = new CheckPoint();
        fieldsM[widthM-2+widthM*(heightM-2-NjamRandom(2))] = new CheckPoint();
    }

    xPosM = 5 + level;
    if (xPosM > 11)
        xPosM = 11;
    xCursorM = xStartM = xPosM ;
    yCursorM = yStartM = 0;
    yPosM = 0;
    flowM = flowDelay;
    flowFromM = dUp;
}
//-----------------------------------------------------------------------------
void Map::RenderNextShape(Shape *s)
{
    const int cell = 60;
    SDL_Rect r;
    NjamSetRect(r, xoffM+cell*xCursorM, yoffM+cell*yCursorM, 14, 14);
    Uint32 yellow = SDL_MapRGB(Screen->format, 255, 255, 0);
    SDL_FillRect(Screen, &r, yellow);
    s->Render(xoffM+cell*xCursorM, yoffM+cell*yCursorM, 4);
}
//-----------------------------------------------------------------------------
void RenderBackground(int housex);  // in main.cpp
void Map::Render()
{
    SDL_Rect r;
    RenderBackground(xStartM*60 - 27);  // 633 = 11
    SDL_Surface *tank = Data->getGfx("tank", "data/tank.png");
    NjamSetRect(r, 12, 529);
    SDL_BlitSurface(tank, 0, Screen, &r);

    const Uint32 black = SDL_MapRGB(Screen->format, 0, 0, 0);

    bool showGrid;
    if (config().getValue("show_grid", showGrid) && showGrid)
    {
        for (int x=0; x<widthM; x++)
        {
            for (int y=0; y<heightM; y++)
            {
                int w = 61;
                int h = 61;
                if (x == widthM-1)
                    w = 60;
                if (y == heightM-1)
                    h = 60;
                NjamSetRect(r, x*60+xoffM, y*60+yoffM, w, h);
                Box(Screen, r, 1, 30, 30, 30);
            }
        }
    }

    // render pipes
    for (int x=0; x<widthM; x++)
        for (int y=0; y<heightM; y++)
            if (fieldsM[y*widthM+x])
                fieldsM[y*widthM+x]->Render(x*60+xoffM, y*60+yoffM, 1);

    // render beneath the house
    NjamSetRect(r, xStartM*60+36, 94, 8, 17);
    SDL_FillRect(Screen, &r, black);

    const int cell = 60;
    const int w = 6;
    if (flowM > -15)
    {
        int h = 16+flowM;
        if (h > 16)
            h = 16;
        const Uint32 water = SDL_MapRGB(Screen->format, 0, 180, 250);
        NjamSetRect(r, xStartM*cell+xoffM+(cell-w)/2, yoffM-15, w, h);
        SDL_FillRect(Screen, &r, water);
    }

    // render cursor
    NjamSetRect(r, xoffM+cell*xCursorM, yoffM+cell*yCursorM, cell, cell);
    Box(Screen, r, 2, 255, 255, 0);

    // walk
    if (flowM < -50)
    {
        SDL_Surface *human = Data->getGfx("human");
        static int frame = 0;
        double p = 6 - ((frame++ / 5) % 7);
        SDL_Rect src, dest;
        NjamSetRect(src, (int)(p*7.5), 0, 8, 13);
        NjamSetRect(dest, xStartM*60 - 10 + (50+flowM)/5, 81);  // 5 = speed
        SDL_BlitSurface(human, &src, Screen, &dest);
    }
}
//-----------------------------------------------------------------------------
bool Map::PutShape(Shape *shape)
{
    Shape *s = fieldsM[yCursorM*widthM+xCursorM];
    if (s && !s->IsDestroyable())
    {
        PlaySound("metal");
        return false;
    }
    PlaySound("drop");
    if (s)
        delete s;
    fieldsM[yCursorM*widthM+xCursorM] = shape;
    return true;
}
//-----------------------------------------------------------------------------
// -1 game over
//  0-xxx points
int Map::AdvanceFlow()
{
    flowM++;

    if (flowM == -50)
        CheckMusic(Data->getMusic("flow"));

    if (flowM < 0)   // delay
        return 0;

    // find the current field, if NONE we're DONE, return false;
    Shape *curr = fieldsM[yPosM*widthM + xPosM];
    if (!curr)
        return -1;

    if (flowM == 0) // only once at level start
    {
        if (!curr->HasDirection(dUp))
            return -1;
    }

    if (curr->SetFill(flowM, flowFromM))  // full - go to next
    {
        Direction d = curr->LeaksTo();
        int xoff = 0, yoff = 0;
        switch (d)
        {
            case dLeft:  xoff = -1; break;
            case dRight: xoff =  1; break;
            case dUp:    yoff = -1; break;
            case dDown:  yoff =  1; break;
        };
        if (xPosM + xoff < 0 || xPosM + xoff >= widthM ||
            yPosM + yoff < 0 || yPosM + yoff >= heightM)
            return -1;

        int newpos = (xPosM+xoff + (yPosM+yoff) * widthM);
        if (fieldsM[newpos] == 0 ||
            !fieldsM[newpos]->HasDirection((Direction)(3-d)))
            return -1;

        xPosM += xoff;
        yPosM += yoff;
        flowM = 0;
        flowFromM = (Direction)(3 - d);
    }

    if (curr->GetSpeed() / 2 == flowM)
        return curr->GetPoints();

    return 0;
}
//-----------------------------------------------------------------------------
void Map::GetPos(int &x, int &y)
{
    x = xPosM*60+50;
    y = yPosM*60+120;
}
//-----------------------------------------------------------------------------
bool Map::LevelComplete()   // flow reached the end pos
{
    // find the current field, if end of map return true
    Shape *s = fieldsM[(heightM-1)*widthM];
    if (xPosM == 0 && yPosM == 6 && s && dDown == s->LeaksTo()
        && flowM >= s->GetSpeed())
    {
        for (int i=0; i<heightM*widthM; i++)
        {
            s = dynamic_cast<CheckPoint *>(fieldsM[i]);
            if (s && s->HasDirection(dLeft))    // should not have when full
                return false;
        }
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
