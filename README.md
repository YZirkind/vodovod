# About

Vodovod is a single-player action puzzle game. You get a limited number of pipes on each level, 
and need to combine them to lead the water from the house at the top of the screen to the
storage tank at the bottom. It is similar to the games Pipe Mania and Pipe Dream.

Vodovod is a [free](https://www.gnu.org/philosophy/free-sw.html) game, released under GNU GPL license.

Graphics and programming are done by Milan Babuškov.
This is an over-weekend project for now, so don't expect miracles.
However, game is completely playable.

The goal of the game is to reach the highest possible score.
You get a limited number of pipes on each level and need to combine them to lead the water
from the house at the top of the screen to the storage tank at the bottom. For each pipe water goes through,
you get 20 points and if you fill the cross-pipe both ways, you get 60 points.

At end of each level, you are awarded depending on the skill level:
* Beginner: 100 points
* Toolman: 100 points + number of pipes remaining
* Master plumber: 100 points + 2 x number of pipes remaining

Some of the levels also have obstacles where you cannot place pipes.
The game is playable with joystick/gamepad: just move it and press buttons when you go to Options -> Configure controls


# Depends On

* SDL 1.2
* SDL_image
* SDL_mixer

# To Compile

Unpack the archive and type: make

This should build the executable. Run it with ./vodovod

Have fun :)

# History

Originally written by Milan Babuškov <mbabuskov@yahoo.com>

Currently maintained by David A. Redick <david.a.redick@gmail.com>

And based off the vodovod 1.10 source code from
http://ba.mirror.garr.it/OpenBSD/distfiles/vodovod-1.10-src.tar.gz

The original home page was hosted by [Gna!](https://en.wikipedia.org/wiki/Gna%21) which went offline circa 2017.

The home page for this maintaince fork is:
https://github.com/david-redick/vodovod/


# Change Log

* Renamed `NjamSetRect` to `SDL_Rect_set` as part of prep work for SDL2 migration.
* Removed the Microsoft Windows specific code.
* Supports the XDG_CONFIG_HOME specification (aka `$HOME/.config/vodovod`).  The previous `$HOME/.vodovod` will be moved if it exists
