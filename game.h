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
#ifndef AB_GAME_H
#define AB_GAME_H

#include <deque>
#include "keys.h"
#include "sutils.h"
#include "njamfont.h"
#include "effects.h"
#include "map.h"
//#include "music.h"
//#include "sound.h"
//-----------------------------------------------------------------------------
class Game
{
public:

private:
    Keyboard kbdM;
    NjamFont fontM;
    Map mapM;
    std::deque<Shape *> queueM;
    bool floodM;

    //Sound sfxDropM;
    //Sound sfxLineM;
    //Music sfxRotateM;

    int currentLevelM;
    int pointsM;
    int pipesLeftM;
    int difficultyM;

    void RenderScene(bool flip = true);
    void ClearQueue();
    void AddShapeToQueue();
    void SetupGame();
    void SetupLevel();
    void Play();
    bool ProcessKeyboard();
    void Drop();
    bool Flow();

    void CheckHiscore();
    void getName(char *name);
    void Message(const std::string& text, bool wait = true);
public:
    Effects specialEffects;

    Game(int difficulty);
    ~Game();
};
//-----------------------------------------------------------------------------
#endif
