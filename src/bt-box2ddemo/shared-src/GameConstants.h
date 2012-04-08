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
#define HALFPI PI / 2
#define TWOPI 2 * PI

#define WORLD_LEFT 0.0f
#define WORLD_BOTTOM 0.0f
#define WORLD_TOP 40.0f
#define WORLD_RIGHT 24.0f

#define GAMESTATE_NONE 0
#define GAMESTATE_READY 1
#define GAMESTATE_RUNNING 2
#define GAMESTATE_LOADING 3

#define GAMEOBJECT_TYPE_NONE 0
#define GAMEOBJECT_TYPE_BALL 1

#define PHYSICS_TIMESTEP 0.008888f
#define BOX2D_POSITION_ITERATIONS 5
#define BOX2D_VELOCITY_ITERATIONS 5
#define WORLD_GRAVITY -9.8f

#define COLLISION_BITS_STATIC 0x0001
#define COLLISION_BITS_LEVEL_OBJECT 0x0002
#define COLLISION_BITS_BALL 0x0004

// ball
#define BALL_RADIUS 1.0f

#define MAX_GAMEOBJECTS 100

// define max score as just under 1 billion, so it's 9 digits and fits into an S32/U32
#define MAX_SCORE 999999999

// preferences
#define PREF_SOUND_ENABLED "sound_enabled"
#define PREF_VIBES_ENABLED "vibes_enabled"
#define PREF_SHOW_FPS "show_fps"

#endif /* GAMECONSTANTS_H_ */
