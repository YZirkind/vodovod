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
//------------------------------------------------------------------------------
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "config.h"
#include <climits>
//------------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------
SDLKey getKey(taAction action)
{
    std::string keys[6] = { "LEFT  ", "RIGHT ", "DOWN  ", "UP    ", "DROP  ", "FLOOD " };
    int key;
    if (!config().getValue(keys[(int)action], key))
        return SDLK_LAST;
    else
        return (SDLKey)key;
}
//-----------------------------------------------------------------------------
Config& config()
{
    static Config c;
    return c;
}
//-----------------------------------------------------------------------------
//! return true if value exists, false if not
bool Config::getValue(string key, string& value)
{
    if (dataM.find(key) == dataM.end())
        return false;

    value = dataM[key];
    return true;
}
//-----------------------------------------------------------------------------
bool Config::getValue(std::string key)
{
    bool b;
    if (getValue(key, b))
        return b;
    else
        return true;    // all options are ON by default
}
//-----------------------------------------------------------------------------
bool Config::getValue(string key, int& value)
{
    string s;
    if (!getValue(key, s))
        return false;

    stringstream ss;
    ss << s;
    ss >> value;
    return true;
}
//-----------------------------------------------------------------------------
bool Config::getValue(string key, double& value)
{
    string s;
    if (!getValue(key, s))
        return false;

    stringstream ss;
    ss << s;
    ss >> value;
    return true;
}
//-----------------------------------------------------------------------------
bool Config::getValue(string key, bool& value)
{
    string s;
    if (!getValue(key, s))
        return false;

    value = (s == "1");
    return true;
}
//-----------------------------------------------------------------------------
//! return true if value existed, false if not
bool Config::setValue(string key, string value, bool saveIt)
{
    bool ret = (dataM.end() != dataM.find(key));
    if (ret)
        dataM.erase(key);
    dataM[key] = value;
    if (saveIt)
        save();
    return ret;
}
//-----------------------------------------------------------------------------
bool Config::setValue(string key, int value, bool saveIt)
{
    stringstream ss;
    ss << value;
    return setValue(key, ss.str(), saveIt);
}
//-----------------------------------------------------------------------------
bool Config::setValue(string key, double value, bool saveIt)
{
    stringstream ss;
    ss << value;
    return setValue(key, ss.str(), saveIt);
}
//-----------------------------------------------------------------------------
bool Config::setValue(string key, bool value, bool saveIt)
{
    if (value)
        return setValue(key, string("1"), saveIt);
    else
        return setValue(key, string("0"), saveIt);
}
//-----------------------------------------------------------------------------
Config::Config()
{
    load();

    // setup default keys if not available
    std::string names[] = {
        "DOWN  ", "DROP  ", "LEFT  ", "RIGHT ", "UP    ", "FLOOD "
    };

    int keys[] = { 274, 32, 276, 275, 273, 13 };
    for (unsigned int i=0; i<sizeof(names)/sizeof(std::string); ++i)
        if (dataM.find(names[i]) == dataM.end())
            setValue(names[i], keys[i]);

    if (dataM.find("show_grid") == dataM.end())
        setValue("show_grid", false);
    if (dataM.find("fullscreen") == dataM.end())
        setValue("fullscreen", false);
}
//-----------------------------------------------------------------------------
Config::~Config()
{
    save();
}
//-----------------------------------------------------------------------------
bool Config::save()
{
    char vodovod_conf[PATH_MAX] = "vodovod.conf";

    char *home = getenv("HOME");
    if (home != NULL)
    {
        snprintf(vodovod_conf, sizeof(vodovod_conf), "%s/.vodovod", home);
        mkdir(vodovod_conf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    strncat(vodovod_conf, "/vodovod.conf", sizeof(vodovod_conf));

    std::ofstream file(vodovod_conf);
    if (!file)
        return false;

    file << "Abandoned Bricks configuration file." << endl << endl << "[Settings]" << endl;
    for (map<string, string>::const_iterator it = dataM.begin(); it != dataM.end(); ++it)
    {
        file << (*it).first << "=" << (*it).second << endl;
    }
    file.close();
    return true;
}
//-----------------------------------------------------------------------------
// this gets called from main() so we're sure config.ini is in the right place
bool Config::load()
{
    char vodovod_conf[PATH_MAX] = "vodovod.conf";

    char *home = getenv("HOME");
    if (home != NULL)
        snprintf(vodovod_conf, sizeof(vodovod_conf), "%s/.vodovod/vodovod.conf", home);

    std::ifstream file(vodovod_conf);
    if (!file)
        return false;

    // I had to do it this way, since standard line << file, doesn't work good if data has spaces in it.
    std::stringstream ss;       // read entire file into string buffer
    ss << file.rdbuf();
    std::string s(ss.str());

    dataM.clear();
    while (true)
    {
        string::size_type t = s.find('\n');
        if (t == string::npos)
            break;

        string line = s.substr(0, t);
        s.erase(0, t+1);

        string::size_type p = line.find('=');
        if (p == string::npos)
            continue;

        string key = line.substr(0, p);
        line.erase(0, p + 1);
        line.erase(line.find_last_not_of(" \t\n\r")+1); // right trim

        setValue(key, line, false);
    }

    file.close();
    return true;
}
//-----------------------------------------------------------------------------
