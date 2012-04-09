/*
 * GameUtil.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rgreen
 */

#ifndef GAMEUTIL_H_
#define GAMEUTIL_H_

#include <batterytech/primitives.h>
#include <batterytech/Context.h>
#include "World.h"

class GameUtil {
public:
	GameUtil();
	virtual ~GameUtil();
	static F32 screenToWorldX(S32 screenX, Context *context);
	static F32 screenToWorldY(S32 screenY, Context *context);
	static S32 worldToScreenX(F32 worldX, Context *context);
	static S32 worldToScreenY(F32 worldY, Context *context);
	static GameObject* findObjectIntersection(F32 worldX, F32 worldY, World *world);

};

#endif /* GAMEUTIL_H_ */
