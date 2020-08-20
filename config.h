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
//! Class that handles reading/writing of configuration options
//! and provides interface to all interested parties
//
#ifndef AB_CONFIG_H
#define AB_CONFIG_H

#include "SDL.h"
#include <map>
#include <vector>
#include <string>
//---------------------------------------------------------------------------------------
enum taAction { taLeft, taRight, taDown, taUp, taDrop, taFlood };
//---------------------------------------------------------------------------------------
SDLKey getKey(taAction action);
//---------------------------------------------------------------------------------------
//! Do not instantiate objects of this class. Use config() function (see below).
class Config
{
public:
	std::string get_config_dir();
	void move_legacy_config(std::string config_dir);

    bool save();
    bool load();

    // return false if value exists, false if not
    bool getValue(std::string key, std::string& value);
    bool getValue(std::string key, int& value);
    bool getValue(std::string key, double& value);
    bool getValue(std::string key, bool& value);

    bool getValue(std::string key);

    // return false if value existed, false if not. Pass false in saveIt
    // to prevent saving the config file before setValue() returns (f. ex.
    // when a number of items are being stored at one time).
    bool setValue(std::string key, std::string value, bool saveIt = false);
    bool setValue(std::string key, int value, bool saveIt = false);
    bool setValue(std::string key, double value, bool saveIt = false);
    bool setValue(std::string key, bool value, bool saveIt = false);

    Config();
    ~Config();
private:
    std::map<std::string, std::string> dataM;       //! key -> value
};

Config& config();
//---------------------------------------------------------------------------------------
#endif
