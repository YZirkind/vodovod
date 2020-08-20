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
// For now we're keeping the high scores with the configuration.
#include "config.h"
//-----------------------------------------------------------------------------
HiScores &hiScores()
{
    static HiScores h;
    return h;
}
//-----------------------------------------------------------------------------
std::string HiScores::get_high_score_dir() {
	// For now we're keeping the high scores with the configuration.
	// Should be /var/games/vodovod or $HOME/.local/share/vodovod
	// Depending on local or system install.
	return config().get_config_dir();
}
//-----------------------------------------------------------------------------
void HiScores::move_legacy_dir(std::string high_score_dir) {
	config().move_legacy_config(high_score_dir);
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
	std::string high_score_dir = this->get_high_score_dir();
	this->move_legacy_dir(high_score_dir);

	std::string path_to_high_scores = high_score_dir + std::string("hiscore.dat");

    FILE *fp = fopen(path_to_high_scores.c_str(), "r");
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
	std::string high_score_dir = this->get_high_score_dir();
	std::string path_to_high_scores = high_score_dir + std::string("hiscore.dat");

    // save to file
    // format: NAME#POINTS#SWAPS#
    FILE *fp = fopen(path_to_high_scores.c_str(), "w+");
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
