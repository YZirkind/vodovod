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
#ifndef AB_MAP_H
#define AB_MAP_H

#include <vector>
//-----------------------------------------------------------------------------
typedef enum { dLeft = 0, dUp, dDown, dRight, dNone, dFull } Direction;
//-----------------------------------------------------------------------------
class Shape     // empty shape
{
protected:
    int fillM;
    Direction flowFromM, flowToM;
public:
    Shape();
    virtual void Render(int x, int y, int scale = 1);
    virtual bool HasDirection(Direction d);
    virtual int GetSpeed();
    virtual bool SetFill(int fill, Direction flowFrom);
    virtual int GetPoints();
    Direction LeaksTo();
    virtual bool IsDestroyable();
};
//-----------------------------------------------------------------------------
class RegularShape: public Shape
{
private:
    Direction directionsM[4];
public:
    RegularShape(char which);
    virtual void Render(int x, int y, int scale = 1);
    virtual bool HasDirection(Direction d);
    virtual bool SetFill(int fill, Direction flowFrom);
    virtual int GetPoints();
};
//-----------------------------------------------------------------------------
class CheckPoint: public RegularShape
{
public:
    CheckPoint();
    virtual void Render(int x, int y, int scale = 1);
    virtual int GetPoints();
    virtual bool IsDestroyable();
};
//-----------------------------------------------------------------------------
class SlowShape: public RegularShape
{
public:
    SlowShape();
    virtual int GetSpeed();
    virtual void Render(int x, int y, int scale = 1);
    virtual int GetPoints();
};
//-----------------------------------------------------------------------------
class Obstacle: public Shape
{
public:
    Obstacle();
    virtual void Render(int x, int y, int scale = 1);
};
//-----------------------------------------------------------------------------
class Map
{
public:
    Map(int w, int h, int x, int y);
    ~Map();

    void Setup(int level, int flowDelay);
    void Render();
    void RenderNextShape(Shape *s);
    bool PutShape(Shape *shape);
    int AdvanceFlow();
    bool LevelComplete();   // flow reached the end pos
    void MoveCursor(int byX, int byY);
    void GetPos(int &x, int &y);

private:
    std::vector<Shape *> fieldsM;
    Direction flowFromM;
    int flowM;
    int xCursorM, yCursorM;
    int xStartM, yStartM;
    int xPosM, yPosM;
    int xoffM, yoffM;   // map offset on screen
    int widthM, heightM;
};
//-----------------------------------------------------------------------------
#endif
