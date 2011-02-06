/*
 * GameConstants.h
 *
 *  Created on: Oct 30, 2010
 *      Author: rgreen
 */

#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

// angle math
#define PI 3.14159265f
#define HALFPI PI / 2
#define TWOPI 2 * PI
// float error tolerance
#define EPSILON 0.0001f

#define WORLD_LEFT 0.0f
#define WORLD_BOTTOM 0.0f
#define WORLD_TOP 40.0f
#define WORLD_RIGHT 24.0f

#define GAMESTATE_NONE 0
#define GAMESTATE_READY 1
#define GAMESTATE_RUNNING 2
#define GAMESTATE_LOADING 3

#define GAMEOBJECT_TYPE_NONE 0
#define GAMEOBJECT_TYPE_PINBALL 1

#define PHYSICS_TIMESTEP 0.016666f
#define BOX2D_POSITION_ITERATIONS 15
#define BOX2D_VELOCITY_ITERATIONS 15
#define WORLD_GRAVITY -9.8f

#define COLLISION_BITS_STATIC 0x0001
#define COLLISION_BITS_LEVEL_OBJECT 0x0002
#define COLLISION_BITS_PINBALL 0x0004

// pinball
#define PINBALL_RADIUS 1.0f

#define MAX_PINBALLS 100
#define MAX_FLIPPERS 10
#define MAX_GAMEOBJECTS 50

#define PINBALL_MAX_REF_SPEED 10.0f

// define max score as just under 1 billion, so it's 9 digits and fits into an S32/U32
#define MAX_SCORE 999999999

// multiplayer

#define NETWORK_MESSAGE_TYPE_GAMESTATE 0
#define NETWORK_MESSAGE_TYPE_PINBALL 1
#define NETWORK_MESSAGE_TYPE_VIBRATION 2

// editor stuff
#define EDITOR_MAX_DRAWPOINTS 300
#define EDITOR_CLOSEST_DRAWPOINT 0.25f
#define EDITOR_SNAP 0.1f
#define EDITOR_MODIFY_DISTANCE 0.001f

// preferences
#define PREF_SOUND_ENABLED "sound_enabled"
#define PREF_VIBES_ENABLED "vibes_enabled"
#define PREF_SHOW_FPS "show_fps"

#endif /* GAMECONSTANTS_H_ */
