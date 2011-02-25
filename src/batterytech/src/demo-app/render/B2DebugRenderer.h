/*
 * B2DebugRenderer.h
 *
 *  Created on: Oct 30, 2010
 *      Author: rgreen
 */

#ifndef B2DEBUGRENDERER_H_
#define B2DEBUGRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <batterytech/math/Vec2f.h>
#include <batterytech/util/ManagedArray.h>

class Context;
class World;

class B2DebugRenderer : Renderer {
public:
	B2DebugRenderer(Context *context);
	virtual ~B2DebugRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(World *world);
private:
	void renderPolyShape(b2Body *body, b2PolygonShape *polyShape);
	void renderCircleShape(b2Body *body, b2CircleShape *shape);
	void renderOrientationLine(b2Body *body);
	void renderLine(F32 x, F32 y, F32 angle, F32 length);
	void renderDrawLines(ManagedArray<Vec2f> *points);
	Context *context;
};

#endif /* B2DEBUGRENDERER_H_ */
