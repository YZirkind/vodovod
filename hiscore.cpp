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
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "hiscore.h"
#include <climits>
#include <cstring>
//-----------------------------------------------------------------------------
HiScores &hiScores()
{
    static HiScores h;
    return h;
}
//-----------------------------------------------------------------------------
void HiScores::addHiscore(std::string name, int level, int points)
{
    HiScore hs;
    hs.name = name;
    hs.level = level;
    hs.points = points;
    list.push_back(hs);
    list.sort();

    while (list.size() > 15)
        list.pop_back();
}
//-----------------------------------------------------------------------------
bool HiScores::canEnter(int score)
{
    return (score > list.back().points);
}
//-----------------------------------------------------------------------------
HiScores::HiScores()
{
    // on Linux use /usr/share/abandoned/hiscore.dat  ?
    FILE *fp = fopen("hiscore.dat", "r");
    if (!fp)
    {
        std::string defaultNames[] = {      // create default hiscore
            "TOMAS",        "PAULO",        "LOUISE",       "JULIAN",       "GEORGE",
            "ENZO",         "JOLAN",        "JAAP",         "MARTIN",       "RICHARD",
            "CLAUS",        "THORSTEN",     "ELLE",         "SABRINA",      "JOSHUA"
        };
        for (int i=0; i<15; ++i)
            addHiscore(defaultNames[i], (int)(15 - i), (15-i)*660);
    }
    else
    {
        // load hiscore from file (if any), format: NAME#POINTS#LEVEL#
        char buff[80];
        int number = 0;
        while (!feof(fp) && number < 15)
        {
            fgets(buff, 80, fp);
            int i = 0, last;

            // name
            std::string name;
            while (buff[i] != '#' && i < 20)
            {
                name += buff[i];
                i++;
            }

            i++;
            last = i;
            while (buff[i] != '#' && i < 70)
                i++;
            if (i >= 70)
                break;
            buff[i] = '\0';
            int points = atoi(buff+last);
            i++;
            last = i;
            while (buff[i] != '#' && i < 70)
                i++;
            if (i >= 70)
                break;
            buff[i] = '\0';
            int level = atoi(buff+last);

            addHiscore(name, level, points);
            number++;
        }
        fclose(fp);
    }
}
//-----------------------------------------------------------------------------
HiScores::~HiScores()
{
    char hiscore_dat[PATH_MAX] = "hiscore.dat";

    char *home = getenv("HOME");
    if (home != NULL)
    {
        snprintf(hiscore_dat, sizeof(hiscore_dat), "%s/.vodovod", home);
        mkdir(hiscore_dat, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    strncat(hiscore_dat, "/hiscore.dat", sizeof(hiscore_dat));


    // save to file
    // format: NAME#POINTS#SWAPS#
    FILE *fp = fopen(hiscore_dat, "w+");
    if (fp)
    {
        for (iterator it = begin(); it != end(); ++it)
            fprintf(fp, "%s#%d#%d#\n", (*it).name.c_str(), (*it).points, (*it).level);
        fclose(fp);
    }
    else
        printf("Unable to write hiscore file!\n");
}
//-----------------------------------------------------------------------------
