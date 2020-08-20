#
# Copyright 2007 Milan Babuskov
# Copyright 2020 David A. Redick
#
# This file is part of Vodovod game
#
# Vodovod is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Vodovod is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Vodovod in file COPYING; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

# directory where the games are installed (/usr, /usr/local, ... or something)
ifndef PREFIX
PREFIX=/usr
endif

PROGRAM = vodovod
OBJECTS = game.o map.o resource.o main.o allmenus.o njamfont.o sutils.o menu.o hiscore.o effects.o config.o keys.o
CC = g++

MY_CFLAGS = `sdl-config --cflags`

%.o:	%.cpp
	$(CC) -c $(MY_CFLAGS) $< -o $@

all:	$(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) `sdl-config --libs` -o$(PROGRAM) $(OBJECTS) -lSDL -lSDL_image -lSDL_mixer

clean:
	$(RM) *.o
	$(RM) $(PROGRAM)
