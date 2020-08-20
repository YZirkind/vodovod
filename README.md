Vodovod is a single-player action puzzle game. You get a limited number of pipes on each level, 
and need to combine them to lead the water from the house at the top of the screen to the
storage tank at the bottom. It is similar to the games Pipe Mania and Pipe Dream.

--

Unpack the archive and type: make

This should build the executable. Run it with ./vodovod

Have fun :)

--

Originally written by Milan Babuskov <mbabuskov@yahoo.com>

Currently maintained by David A. Redick <david.a.redick@gmail.com>
And based off the vodovod 1.10 source code from
http://ba.mirror.garr.it/OpenBSD/distfiles/vodovod-1.10-src.tar.gz

The original home page is currently offline and no signs of coming back up.
http://home.gna.org/vodovod/

The home page for this maintaince fork is:
https://github.com/david-redick/vodovod/

--

# Change Log

* Renamed `NjamSetRect` to `SDL_Rect_set` as part of prep work for SDL2 migration.
* Remove the Microsoft Windows specific code.

