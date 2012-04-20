/*
 * GameUtil.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#ifndef GAMEUTIL_H_
#define GAMEUTIL_H_

#include <batterytech/primitives.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/math/Vector3.h>

using namespace BatteryTech;

class GameObject;
class GameContext;

class GameUtil {
public:
	GameUtil();
	virtual ~GameUtil();
	static Vector3f getScreenCoord(const Vector3f &worldCoord, GameContext *context);
	//static GameObject* pickObject(const Vector2f &screenCoord, Context *context, F32 xySlop);
	//static GameObject** pickObjects(const Vector2f &screenCoord, Context *context, F32 xySlop, S32 *count);
	//static Vector3f castRayToLevel(const Vector2f &screenCoord, Context *context);
	//static Vector3f castRayToGameObject(const Vector2f &screenCoord, GameObject *gameObject, Context *context);
	// uses length of rayVector for ray length
	//static F32 getLineOfSightDistance(const Vector3f &from, const Vector3f &to, Context *context, GameObject *exclusion1, GameObject *exclusion2);
	static GameObject** pickArray;
};

#endif /* GAMEUTIL_H_ */
