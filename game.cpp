/*-----------------------------------------------------------------------------
Copyright 2007 Milan Babuskov
Copyright 2020 David A. Redick

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
#include <string>
#include <sstream>
#include "keys.h"
#include "sutils.h"
#include "hiscore.h"
#include "config.h"
#include "game.h"
#include "resource.h"
extern SDL_Surface *Screen;     // global objects
extern ResourceManager *Data;
//-----------------------------------------------------------------------------
Game::Game(int difficulty):
    fontM("data/font-white.bmp", 8, 14), mapM(13, 7, 10, 110)
{
    difficultyM = difficulty;
    SetupGame();
    SetupLevel();
    Play();
    CheckHiscore();
}
//-----------------------------------------------------------------------------
void Game::ClearQueue()
{
    while (!queueM.empty())
    {
        Shape *s = queueM.front();
        delete s;
        queueM.pop_front();
    }
}
//-----------------------------------------------------------------------------
Game::~Game()
{
    ClearQueue();
}
//-----------------------------------------------------------------------------
void Game::SetupGame()
{
    currentLevelM = 0;
    pointsM = 0;
}
//-----------------------------------------------------------------------------
void Game::AddShapeToQueue()
{
    const char shapes[] = "LJF7-+|";
    Shape *s = new RegularShape(shapes[NjamRandom(7)]);
    queueM.push_back(s);
}
//-----------------------------------------------------------------------------
void Game::SetupLevel()
{
    CheckMusic(Data->getMusic("walk"));

    // rnd
    Uint32 x = (SDL_GetTicks() % 1000);
    while (x-- > 0)
        NjamRandom(1);

    floodM = false;
    currentLevelM++;
    pipesLeftM = 210 - (difficultyM+1)*currentLevelM*10;
    if (pipesLeftM < 86)
        pipesLeftM = 86;

    // clean up field, flowDelay = -200
    mapM.Setup(currentLevelM, -1000 + difficultyM*200);

    ClearQueue();
    for (int i=0; i<6; i++)
        AddShapeToQueue();

    char buffer[20];
    sprintf(buffer, "LEVEL %d", currentLevelM);
    TextEffect *t = new TextEffect(&fontM, 350, 260, std::string(buffer), 0);
    specialEffects.addEffect(t);
}
//-----------------------------------------------------------------------------
bool Game::Flow()
{
    int res = mapM.AdvanceFlow();
    if (mapM.LevelComplete())
    {
        if (Mix_PlayingMusic())
            Mix_HaltMusic();
        PlaySound("levelend");
        int pts = 100 + pipesLeftM*difficultyM;
        pointsM += pts;
        specialEffects.addEffect(new ScoreEffect(&fontM, 80, 560, pts));

        TextEffect *t1 = new TextEffect(&fontM, 351, 240,
            std::string("LEVEL COMPLETE"), -1);
        TextEffect *t2 = new TextEffect(&fontM, 357, 255,
            std::string("PRESS ANY KEY"), -1);
        specialEffects.addEffect(t1);
        specialEffects.addEffect(t2);

        while (NjamGetch(false) != SDLK_LAST)
            ;   // clean up event queue
        while (true)
        {
            Uint32 startTime = SDL_GetTicks();
            RenderScene();
            if (NjamGetch(false) != SDLK_LAST)
                break;
            while (startTime + 34 > SDL_GetTicks())
                SDL_Delay(1);    // target about 30 fps
        }
        specialEffects.removeEffect(t1);
        specialEffects.removeEffect(t2);

        SetupLevel();
        return true;
    }
    if (res == -1)
        return false;
    if (res > 0)
    {
        int x, y;
        mapM.GetPos(x, y);
        specialEffects.addEffect(new ScoreEffect(&fontM, x, y, res));
        pointsM += res;

        if (res == 30)  // slow gives 30 pts
            specialEffects.addEffect(new TextEffect(&fontM, x-25, y, "SLOW", 0));
    }

    return true;
}
//-----------------------------------------------------------------------------
void Game::Play()
{
    while (true)            // check for single player
    {
        Uint32 startTime = SDL_GetTicks();
        int times = (floodM ? 10 : 1);
        bool ok;
        while (times--)
        {
            ok = (ProcessKeyboard() && Flow());
            if (!ok)
                break;
        }
        RenderScene();
        CheckMusic(0);
        if (!ok)
            return;

        while (startTime + 34 > SDL_GetTicks())
            SDL_Delay(1);    // target about 30 fps
    }
}
//-----------------------------------------------------------------------------
void Game::RenderScene(bool flip)
{
    mapM.Render();
    if (queueM.begin() != queueM.end())
        mapM.RenderNextShape(*(queueM.begin()));

    SDL_Rect r;
    SDL_Surface *next = Data->getGfx("next", "data/next.png");
    SDL_Rect_set(r, 363, 533);
    SDL_BlitSurface(next, 0, Screen, &r);

    SDL_Surface *queue = Data->getGfx("queue");
    SDL_Rect_set(r, 424, 532);
    SDL_BlitSurface(queue, 0, Screen, &r);
    int i = 0;
    for (std::deque<Shape *>::iterator it = queueM.begin();
        it != queueM.end(); ++it, ++i)
    {
        (*it)->Render(i*60+427, 534);

        SDL_Rect_set(r, i*60+427, 534, 61, 60);
        Box(Screen, r, 1, 75, 75, 75);
    }

    char buffer[30];
    sprintf(buffer, "LEVEL:  %8d", currentLevelM);
    fontM.WriteTextXY(Screen, 185, 540, buffer);
    sprintf(buffer, "POINTS: %8d", pointsM);
    fontM.WriteTextXY(Screen, 185, 555, buffer);
    sprintf(buffer, "PIPES LEFT: %4d", pipesLeftM);
    fontM.WriteTextXY(Screen, 185, 570, buffer);

    specialEffects.Render();
    if (flip)
        SDL_Flip(Screen);
}
//-----------------------------------------------------------------------------
bool Game::ProcessKeyboard()
{
    SDLKey key;
    bool repeatKeys = true;

    bool pause = false;
    if ((SDL_GetAppState() & (SDL_APPINPUTFOCUS | SDL_APPACTIVE)) != (SDL_APPINPUTFOCUS | SDL_APPACTIVE))
        pause = true;


    // process single keys
    while ((key = kbdM.update(repeatKeys)) != SDLK_LAST)
    {
        repeatKeys = false;     // so we don't influence the repeat-delay

        if (key == SDLK_ESCAPE)
            return false;
        if (key == SDLK_p)
            pause = true;

        if (key == getKey(taUp))
            mapM.MoveCursor( 0, -1);
        else if (key == getKey(taRight))
            mapM.MoveCursor( 1, 0);
        else if (key == getKey(taLeft))
            mapM.MoveCursor(-1, 0);
        else if (key == getKey(taDown))
            mapM.MoveCursor( 0, 1);
        else if (key == getKey(taDrop))
            Drop();
        else if (key == getKey(taFlood))
            floodM = true;
    }

    // process sticky keys (those that can be held down)
    if (kbdM.isKeyDown(getKey(taDown)))
        mapM.MoveCursor( 0, 1);
    if (kbdM.isKeyDown(getKey(taUp)))
        mapM.MoveCursor( 0, -1);
    if (kbdM.isKeyDown(getKey(taRight)))
        mapM.MoveCursor( 1, 0);
    if (kbdM.isKeyDown(getKey(taLeft)))
        mapM.MoveCursor(-1, 0);

    if (pause)
    {
        while (NjamGetch(false) != SDLK_LAST);  // wait to release the key

        TextEffect *t = new TextEffect(&fontM, 350, 240,
            std::string("GAME PAUSED"), -1);
        specialEffects.addEffect(t);

        while (SDLK_p != NjamGetch(false))      // wait to press the key again
        {
            RenderScene();
            SDL_Delay(5);
        }
        specialEffects.removeEffect(t);
    }

    return true;
}
//-----------------------------------------------------------------------------
void Game::Drop()
{
    // drop shape from queue to map if possible, change score
    if (queueM.empty()) // no more pipes
    {
        PlaySound("metal");
        return;
    }

    Shape *s = queueM.front();
    if (mapM.PutShape(s))
    {
        queueM.pop_front();
        if (pipesLeftM > 0)
            pipesLeftM--;
        if (pipesLeftM > 5)
            AddShapeToQueue();
    }
}
//-----------------------------------------------------------------------------
void Game::CheckHiscore()
{
    if (Mix_PlayingMusic())
        Mix_HaltMusic();
    HiScores &hs = hiScores();
    if (!hs.canEnter(pointsM))
    {
        SDL_Delay(300);
        PlaySound("bubbles");
        SDL_Delay(300);
        Message("GAME OVER * PRESS ANY KEY");
        return;
    }
    else
        PlaySound("levelend");

    // ask for name
    char name[30];
    getName(name);
    hs.addHiscore(name, currentLevelM, pointsM);
}
//-----------------------------------------------------------------------------
void Game::getName(char *name)
{
    NjamFont *font = &fontM;
    Uint32 white = SDL_MapRGB(Screen->format, 255, 255, 255);
    Uint32 back = SDL_MapRGB(Screen->format, 100, 50, 0);
    Uint32 t = SDL_GetTicks();
    bool CursorOn = true;

    const int namelen = 19;
    const int yp = 220;

    int fieldw = font->GetCharWidth() * (namelen+2);
    int xp = (800 -  fieldw)/2;
    int textw = font->GetCharWidth() * namelen;
    int fieldh = (int)((double)font->GetCharHeight() * 3.5);
    int textx = xp+font->GetCharWidth();
    int texty = yp+font->GetCharHeight()*2;

    name[0] = '\0';
    while (true)
    {
        SDL_Rect dest;
        SDL_Rect_set(dest, xp, yp, fieldw, fieldh);
        SDL_FillRect(Screen, &dest, 0);
        SDL_Rect_set(dest, xp+1, yp+1, fieldw-2, fieldh-2);
        SDL_FillRect(Screen, &dest, back);
        font->WriteTextXY(Screen, textx, yp+font->GetCharHeight()/2, "*** NEW HISCORE ***");
        SDL_Rect_set(dest, textx, texty, textw, font->GetCharHeight()+1);
        SDL_FillRect(Screen, &dest, 0);
        font->WriteTextXY(Screen, textx+1, texty+1, name);

        int l = 0;  // get len
        while (name[l])
            l++;

        if (SDL_GetTicks() > t + 400)   // make the cursor blink
        {
            t = SDL_GetTicks();
            CursorOn = !CursorOn;
        }

        if (CursorOn)
        {
            SDL_Rect_set(dest, textx + font->GetCharWidth() * l + 2, texty+2, font->GetCharWidth()-2,
                font->GetCharHeight()-2);   // draw cursor
            SDL_FillRect(Screen, &dest, white);
        }

        SDL_Flip(Screen);

        SDLKey key = NjamGetch(false);
        char allowed[] = "0123456789abcdefghijklmnopqrstuvwxyz. :)(!*";
        bool ok = false;
        for (int k=0; allowed[k]; k++)
            if (allowed[k] == key)
                ok = true;

        if (ok && l < 20)
        {
            char c = key;
            if (c >= 'a' && c <= 'z')
                c -= ('a' - 'A');
            name[l] = c;
            name[l+1] = '\0';
        }

        if (key == SDLK_BACKSPACE && l > 0)         // backspace
            name[l-1] = '\0';

        if ((key == SDLK_KP_ENTER || key == SDLK_RETURN) && name[0])    // entered
            break;
    }
}
//-----------------------------------------------------------------------------
void Game::Message(const std::string& text, bool wait)
{
    if (wait)
        while (NjamGetch(false) != SDLK_LAST);  // clear the buffer

    const int screenw = 800;
    int l = text.length();
    int w = fontM.GetCharWidth();

    // render background
    SDL_Rect dest;
    SDL_Rect_set(dest, (screenw-l*w-50)/2, 245, l*w+50, 55);
    SDL_FillRect(Screen, &dest, 0);
    Uint32 FillColor = SDL_MapRGB(Screen->format, 255, 200, 0);
    SDL_Rect_set(dest, (screenw-l*w-50)/2 + 1, 246, l*w+48, 53);
    SDL_FillRect(Screen, &dest, FillColor);

    SDL_Rect_set(dest, (screenw-l*w-30)/2, 255, l*w+30, 35);
    SDL_FillRect(Screen, &dest, 0);

    fontM.WriteTextXY(Screen, (screenw-l*w)/2, 265, text.c_str());
    SDL_Flip(Screen);

    if (wait)
        NjamGetch(true);
}
//-----------------------------------------------------------------------------
