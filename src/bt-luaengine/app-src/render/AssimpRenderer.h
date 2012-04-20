/*
 * AssimpRenderer
 *
 *  Created on: Oct 16, 2011
 *      Author: rgreen
 */

#ifndef ASSIMPRENDERER_H_
#define ASSIMPRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>
#include "../../bt-assimp/include/aiScene.h"
#include <batterytech/render/RenderNode.h>

using namespace BatteryTech;

class RenderItem;
class GameContext;

namespace BatteryTech { class GLAssimpBinding; class GLAssimpMeshBinding;}

#define SKEL_MAX_LINES 50

class AssimpRenderer: public Renderer {
public:
	AssimpRenderer(GameContext *context);
	virtual ~AssimpRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(RenderItem *item, BOOL32 transparent);
    void renderShadow(RenderItem* item);
    void unbind();
        
private:
	ShaderProgram* createShaderProgram(BOOL32 newContext, BOOL32 hwSkinned);
	ShaderProgram* createShadowShaderProgram(BOOL32 newContext, BOOL32 hwSkinned);
	void bindShader(ShaderProgram *shaderProgram, const Vector3f &ecLightDir, const Vector3f &halfplane);
	void setupPointLights(ShaderProgram *shaderProgram, RenderItem *item, Matrix4f mv);
	void renderNode(RenderNode *node, GLAssimpBinding *binding, Matrix4f mv, RenderItem *item, const Vector3f &ecLightDir, const Vector3f &halfplane, BOOL32 transparent);
    
    void renderNodeShadow(RenderNode *node, GLAssimpBinding *binding, RenderItem *item, Matrix4f mv);
	void bindMaterial(RenderItem *item, GLAssimpMeshBinding *meshBinding, GLAssimpBinding *binding);
	void renderSkeleton(RenderItem *item);
	void RenderSkeletonNode(RenderNode* node, const Matrix4f parent);
	void drawLine(const Vector3f& from, const Vector3f& to,	const Vector3f& color);
	GameContext *context;
	ShaderProgram *defaultShaderProgram;
	ShaderProgram *hwSkinShaderProgram;
	ShaderProgram *skelShaderProgram;
	ShaderProgram *currentShaderProgram;
	ShaderProgram *shadowShader;
	ShaderProgram *shadowHwSkinShader;
    GLAssimpMeshBinding *currentMeshBinding;
	BOOL32 lightsBound;
	F32 vPos[SKEL_MAX_LINES * 6];
	F32 vColor[SKEL_MAX_LINES * 8];
	S32 lineCount;
};

#endif /* ASSIMPRENDERER_H_ */
