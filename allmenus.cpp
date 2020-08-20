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
#include <sstream>
#include "SDL_mixer.h"

#include "game.h"
#include "keys.h"
#include "menu.h"
#include "config.h"
#include "hiscore.h"
#include "resource.h"
#include "sutils.h"
#include "allmenus.h"

extern SDL_Surface *Screen;
extern ResourceManager *Data;
//-----------------------------------------------------------------------------
// MAIN MENU
//-----------------------------------------------------------------------------
MainMenu::MainMenu(NjamFont *font_ptr, int x, int y):
    Menu(font_ptr, x, y),
    whiteFontM("data/font-white.bmp", 8, 14)
{
    options.push_back("START NEW GAME");
    options.push_back("OPTIONS");
    options.push_back("HELP");
    options.push_back("EXIT");

    SDL_JoystickEventState(SDL_ENABLE);
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* j = SDL_JoystickOpen(i);
        if (j)
            joysM.push_back(j);
    }

    CheckMusic(Data->getMusic("tune"));
}
//-----------------------------------------------------------------------------
MainMenu::~MainMenu()
{
    for (std::vector<SDL_Joystick *>::iterator it = joysM.begin();
        it != joysM.end(); ++it)
    {
        SDL_JoystickClose(*it);
    }
}
//-----------------------------------------------------------------------------
void RenderBackground(int housex);
void DrawHelp()
{
    RenderBackground(300);

    NjamFont font("data/font-white.bmp", 8, 14);
    int row = 10;
    const int col = 16;
    font.WriteTextColRow(Screen, col, row++, "GOAL OF THE GAME IS TO USE PIPES TO CONNECT THE HOUSE AT THE TOP");
    font.WriteTextColRow(Screen, col, row++, "OF THE SCREEN WITH THE TANK AT THE BOTTOM. YOU HAVE TIME TO ");
    font.WriteTextColRow(Screen, col, row++, "CONSTRUCT THE INITIAL PIPELINE BEFORE THE MAN ENTERS THE HOUSE.");
    row++;
    font.WriteTextColRow(Screen, col, row++, "FOR EACH LEVEL YOU GET A LIMITED NUMBER OF PIPES, SO USE THEM");
    font.WriteTextColRow(Screen, col, row++, "WISELY. IF YOU MAKE A MISTAKE, YOU CAN PLACE ANOTHER PIPE OVER IT.");
    row++;
    row++;
    font.WriteTextColRow(Screen, col, row++, "SPECIALS:");
    row += 2;
    font.WriteTextColRow(Screen, col+10, row, "BLOCKED AREA.   YOU CANNOT PLACE PIPES ON THOSE FIELDS.");
    row += 5;
    font.WriteTextColRow(Screen, col+10, row, "CHECKPOINT.     YOUR PIPELINE MUST GO THROUGH THIS FIELD.");
    row += 5;
    font.WriteTextColRow(Screen, col+10, row, "HOLE IN GROUND. SLOWS DOWN THE FLOW.");
    row += 9;
    font.WriteTextColRow(Screen, col+23, row++, "PRESS ANY KEY TO GO BACK");

    SDL_Surface *brick = Data->getGfx("blocks");
    SDL_Rect src, dest;
    SDL_Rect_set(src, 0, 0, 60, 60);
    SDL_Rect_set(dest, 131, 273);
    SDL_BlitSurface(brick, &src, Screen, &dest);

    SDL_Rect_set(src, 60, 0, 60, 60);
    SDL_Rect_set(dest, 131, 343);
    SDL_BlitSurface(brick, &src, Screen, &dest);

    SDL_Rect_set(dest, 131, 413, 60, 60);
    Box(Screen, dest, 1, 0, 0, 0);
    SlowShape ssh;
    ssh.Render(131, 413);

    SDL_Flip(Screen);
    NjamGetch(true);
}
//-----------------------------------------------------------------------------
bool MainMenu::onEnter()
{
    if (indicator == 0)
    {
        SelectGameTypeMenu sgt(font, 240, 295);
        sgt.start();
    }
    else if (indicator == 1)        // options
    {
        OptionsMenu o(font, 165, 320);
        o.start();
    }
    else if (indicator == 2)
    {
        DrawHelp();
    }
    else                                    // exit
        return false;
    return true;
}
//-----------------------------------------------------------------------------
void MainMenu::render()
{
    SDL_Rect r;

    // render hiscore list
    const int hsx = 420;
    const int hsy = 180;

    SDL_Rect_set(r, hsx, hsy, 38*whiteFontM.GetCharWidth(), 18*whiteFontM.GetCharHeight()+font->GetCharWidth());
    SDL_FillRect(Screen, &r, 0);
    Box(Screen, r, 1, 150, 150, 100);
    font->WriteTextXY(Screen, hsx+80, hsy+4, "TOP 15 PLAYERS");
    whiteFontM.WriteTextXY(Screen, hsx+5, hsy+2*whiteFontM.GetCharHeight(), "   NAME               LEVEL   POINTS");
    SDL_Rect_set(r, hsx+5, hsy+3*whiteFontM.GetCharHeight(), 36*whiteFontM.GetCharWidth(), 1);
    Box(Screen, r, 1, 150, 150, 100);

    HiScores &hs = hiScores();
    int i=1;
    for (HiScores::iterator it = hs.begin(); it != hs.end() && i<=15; ++it, ++i)
    {
        char buffer[80];
        sprintf(buffer, "%2d %-20s %3d %8d", i, (*it).name.c_str(), (*it).level, (*it).points);
        whiteFontM.WriteTextXY(Screen, hsx+5, hsy+(i+2)*whiteFontM.GetCharHeight(), buffer);
    }

    effectsM.Render();

    static int counter = -1;
    counter++;

    std::string text[] = {
        "MILAN BABUSKOV PRESENTS",
        "VODOVOD",
        "VERSION " PROG_VERSION,
        "CONSTRUCT THE PIPELINE BEFORE WATER RUNS FREE",
        "HAVE FUN PLAYING",
        " "
    };
    int items = sizeof(text)/sizeof(std::string);
    const int speed = 210;

    if (counter > speed*items-1)
        counter = 0;
    if (counter % speed != 0)
        return;

    std::string txt = text[counter/speed];
    int p = (800 - txt.length() * whiteFontM.GetCharWidth()) / 2;

    SpecialEffect *t = new ScrollEffect(&whiteFontM, p, 20, txt);
    effectsM.addEffect(t);
}
//-----------------------------------------------------------------------------
// OPTIONS MENU
//-----------------------------------------------------------------------------
OptionsMenu::OptionsMenu(NjamFont *font_ptr, int x, int y)
    : Menu(font_ptr, x, y)
{
    bool fs    = config().getValue("fullscreen");
    bool grid  = config().getValue("show_grid");
    options.push_back(fs            ? "FULLSCREEN  ON" : "FULLSCREEN  OFF");
    options.push_back(grid          ? "GRID        ON" : "GRID        OFF");

    int vol = 90;
    config().getValue("musicVolume", vol);
    std::stringstream ssm, sss;
    ssm << "MUSIC       " << vol << " %";
    options.push_back(ssm.str());

    vol = 90;
    config().getValue("soundVolume", vol);
    sss << "SFX         " << vol << " %";
    options.push_back(sss.str());

    options.push_back("CONFIGURE CONTROLS");
    options.push_back("BACK");
}
//-----------------------------------------------------------------------------
bool OptionsMenu::onEnter()
{
    if (indicator == 4)
    {
        KeyboardMenu k(font, 300, 430);
        k.start(27);
        return true;    // stay in options menu
    }

    bool b;
    switch (indicator)
    {
        case 0:
            b = !config().getValue("fullscreen");
            InitVideo(b);
            options[0] = (b ? "FULLSCREEN  ON" : "FULLSCREEN  OFF");
            config().setValue("fullscreen", b);
            break;
        case 1:
            b = !config().getValue("show_grid");
            options[1] = (b ? "GRID        ON" : "GRID        OFF");
            config().setValue("show_grid", b);
            break;
        case 2:
        case 3:
        {
            int vol;
            if (indicator == 2)
                vol = MusicVolume(-10);
            else
                vol = SoundVolume(-10);
            std::stringstream ss;
            ss  << (indicator == 2 ? "MUSIC       " : "SFX         ")
                << vol << " %";
            options[indicator] = ss.str();
            break;
        }
        default:
            return false;
    }
    return true;        // stay in menu
};
//-----------------------------------------------------------------------------
KeyboardMenu::KeyboardMenu(NjamFont *font_ptr, int x, int y):
    Menu(font_ptr, x, y)
{
    std::string keys[6] = { "LEFT  ", "RIGHT ", "DOWN  ", "UP    ", "DROP  ", "FLOOD " };
    for (int i=0; i<6; ++i)
    {
        int key;
        if (!config().getValue(keys[i], key))
            key = 0;
        std::string keyname = KeyNames::keyNamesProvider().getKeyName(key);
        std::string opt = keys[i] + ":" + keyname;
        options.push_back(opt);
    }
    options.push_back("DONE");
}
//-----------------------------------------------------------------------------
bool KeyboardMenu::onEnter()
{
    if (indicator == 6)
        return false;

    int key = indicator%6;
    std::string keys[6] = { "LEFT  ", "RIGHT ", "DOWN  ", "UP    ", "DROP  ", "FLOOD " };

    // ask user to press new key
    SDLKey k = Message("PRESS KEY FOR " + keys[key]);
    if (k == SDLK_ESCAPE)
        return true;

    // assing key to action via config().setValue()
    config().setValue(keys[key], k);

    // change menu item's text
    std::string keyname = KeyNames::keyNamesProvider().getKeyName(k);
    options[indicator] = keys[key] + ":" + keyname;
    return true;
}
//-----------------------------------------------------------------------------
// GAME TYPE MENU
//-----------------------------------------------------------------------------
SelectGameTypeMenu::SelectGameTypeMenu(NjamFont *font_ptr, int x, int y)
    : Menu(font_ptr, x, y)
{
    options.push_back("SKILL LEVEL");
    options.push_back("BEGINNER");
    options.push_back("TOOLMAN");
    options.push_back("MASTER PLUMBER");     // only 2 brick types per level + trash at bottom
    options.push_back("BACK");

    firstIsTitleM = true;
    indicator = 1;
}
//-----------------------------------------------------------------------------
bool SelectGameTypeMenu::onEnter()
{
    if (indicator < 4)
    {
        Game g(indicator-1);      // 1 player

        // play main menu music when done
        CheckMusic(Data->getMusic("tune"));
    }
    return false;
}
//-----------------------------------------------------------------------------
