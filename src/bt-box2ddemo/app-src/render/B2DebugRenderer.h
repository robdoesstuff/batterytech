/*
 * B2DebugRenderer.h
 *
 *  Created on: Oct 30, 2010
 *      Author: rgreen
 */

#ifndef B2DEBUGRENDERER_H_
#define B2DEBUGRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <bt-box2d/Dynamics/b2Body.h>
#include <bt-box2d/Collision/Shapes/b2CircleShape.h>
#include <bt-box2d/Collision/Shapes/b2PolygonShape.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/util/ManagedArray.h>

class World;
class GameContext;

using namespace BatteryTech;

class B2DebugRenderer : Renderer {
public:
	B2DebugRenderer(GameContext *context);
	virtual ~B2DebugRenderer();
	virtual void init(BOOL32 newGameContext);
	virtual void render(World *world);
private:
	void renderPolyShape(b2Body *body, b2PolygonShape *polyShape);
	void renderCircleShape(b2Body *body, b2CircleShape *shape);
	void renderOrientationLine(b2Body *body);
	void renderLine(F32 x, F32 y, F32 angle, F32 length);
	void renderDrawLines(ManagedArray<Vector2f> *points);
	GameContext *context;
};

#endif /* B2DEBUGRENDERER_H_ */
