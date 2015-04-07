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

/*#ifndef BTDEBUGRENDERER_H_
#define BTDEBUGRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/Context.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include "BatchSpriteRenderer.h"
//#include <LinearMath/btIDebugDraw.h>
#include <batterytech/render/ShaderProgram.h>

using namespace BatteryTech;

#define BT_DEBUG_MAX_LINES 10000

class BtDebugRenderer: public Renderer, public btIDebugDraw {
public:
	BtDebugRenderer(Context *context);
	virtual ~BtDebugRenderer();
	virtual void init(BOOL32 newContext);
    virtual void   drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
    virtual void   drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    virtual void   reportErrorWarning(const char* warningString);
    virtual void   draw3dText(const btVector3& location,const char* textString);
    virtual void   setDebugMode(int debugMode);
    virtual int      getDebugMode() const { return m_debugMode;}
    void start();
    void end();
private:
    int m_debugMode;
	Context *context;
	ShaderProgram *shaderProgram;
	F32 vPos[BT_DEBUG_MAX_LINES * 6];
	F32 vColor[BT_DEBUG_MAX_LINES * 8];
	S32 lineCount;
};

#endif //BALLRENDERER_H_ */
