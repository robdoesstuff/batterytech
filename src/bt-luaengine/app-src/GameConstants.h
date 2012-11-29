/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
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

#define DEFAULT_GRAVITY -9.8f

#define MAX_PICKOBJECTS 10

#define MAX_JOINTS 300
#define MAX_GAMEOBJECTS 300
#define MAX_SCREENCONTROLS 100
#define MAX_RENDERITEMS 300
#define MAX_LOCALLIGHTS 100
#define MAX_PARTICLE_EMITTER 10
#define MAX_PARTICLES 400
#define MAX_SOUND_LOAD_PENDING 50

#define FONT_TAG_UI "ui"

// preferences
#define PREF_SOUND_ENABLED "sound_enabled"
#define PREF_VIBES_ENABLED "vibes_enabled"
#define PREF_SHOW_FPS "show_fps"

#endif /* GAMECONSTANTS_H_ */
