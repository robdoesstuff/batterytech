/*
 * GameConstants.h
 *
 *  Created on: Oct 30, 2010
 *      Author: rgreen
 */

#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

// angle math
#ifndef PI
#define PI 3.14159265f
#endif
#define HALFPI 1.570796325f
#define TAU 6.2831853f

#define GAMESTATE_NONE 0
#define GAMESTATE_READY 1
#define GAMESTATE_RUNNING 2
#define GAMESTATE_LOADING 3

#define GAMEMODE_GARAGE 0
#define GAMEMODE_RACE_SELECTION 1
#define GAMEMODE_RACE 2
#define GAMEMODE_MAINMENU 3
#define GAMEMODE_SERVICE 4

#define WORLD_GRAVITY -9.8f

#define MAX_PICKOBJECTS 10

#define MAX_GAMEOBJECTS 300
#define MAX_SCREENCONTROLS 50
#define MAX_RENDERITEMS 300
#define MAX_LOCALLIGHTS 100

#define FONT_TAG_UI "ui"

// preferences
#define PREF_SOUND_ENABLED "sound_enabled"
#define PREF_VIBES_ENABLED "vibes_enabled"
#define PREF_SHOW_FPS "show_fps"

#endif /* GAMECONSTANTS_H_ */
