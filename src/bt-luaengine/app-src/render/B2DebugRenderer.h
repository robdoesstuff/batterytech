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

#ifndef B2DEBUGRENDERER_H_
#define B2DEBUGRENDERER_H_

#ifdef BATTERYTECH_INCLUDE_BOX2D

#include <batterytech/render/Renderer.h>
#include <bt-box2d/Dynamics/b2Body.h>
#include <bt-box2d/Collision/Shapes/b2CircleShape.h>
#include <bt-box2d/Collision/Shapes/b2PolygonShape.h>
#include <batterytech/math/Vector2.h>
#include <batterytech/util/ManagedArray.h>
#include <bt-box2d/Common/b2Draw.h>

class World;
class GameContext;

using namespace BatteryTech;

#define B2DEBUG_BUFFER_LENGTH 2048

class B2DebugRenderer : Renderer, b2Draw {
public:
	B2DebugRenderer(GameContext *context);
	virtual ~B2DebugRenderer();
	virtual void init(BOOL32 newGameContext);
	// b2Draw functions
	/// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf);
	virtual void render(World *world);
private:
	void addLine(Vector3f p1, Vector3f p2, Vector4f color);
	void flush();
	GameContext *context;
	Vector3f posBuffer[B2DEBUG_BUFFER_LENGTH];
	Vector4f colorBuffer[B2DEBUG_BUFFER_LENGTH];
	S32 bufferUsed;
};

#endif

#endif /* B2DEBUGRENDERER_H_ */
