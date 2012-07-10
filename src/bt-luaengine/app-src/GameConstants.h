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

#define WORLD_GRAVITY -9.8f

#define MAX_PICKOBJECTS 10

#define MAX_GAMEOBJECTS 300
#define MAX_SCREENCONTROLS 100
#define MAX_RENDERITEMS 300
#define MAX_LOCALLIGHTS 100
#define MAX_PARTICLE_EMITTER 10
#define MAX_PARTICLES 400

#define FONT_TAG_UI "ui"

// preferences
#define PREF_SOUND_ENABLED "sound_enabled"
#define PREF_VIBES_ENABLED "vibes_enabled"
#define PREF_SHOW_FPS "show_fps"

#endif /* GAMECONSTANTS_H_ */
