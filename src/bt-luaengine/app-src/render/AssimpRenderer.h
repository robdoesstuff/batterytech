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

#ifndef ASSIMPRENDERER_H_
#define ASSIMPRENDERER_H_

#include <batterytech/render/Renderer.h>
#include <batterytech/primitives.h>
#include <batterytech/platform/platformgl.h>
#include <batterytech/render/ShaderProgram.h>
#include <bt-assimp/include/aiScene.h>
#include <batterytech/render/RenderNode.h>

using namespace BatteryTech;

class RenderItem;
class GameContext;

namespace BatteryTech { class GLAssimpBinding; class GLAssimpMeshBinding;}

#define SKEL_MAX_LINES 50

struct AssimpShaderConfig {
	AssimpShaderConfig() {
		this->hwAccel = FALSE;
		this->withRGBAShadowmap = FALSE;
		this->withDirectionalLight = FALSE;
		this->withFog = FALSE;
		this->pointLightCount = 0;
		this->vertexLighting = TRUE;
	}
	AssimpShaderConfig(BOOL32 hwAccel, BOOL32 withRGBAShadowmap, BOOL32 withDirectionalLight, BOOL32 withFog, S32 pointLightCount) {
		this->hwAccel = hwAccel;
		this->withRGBAShadowmap = withRGBAShadowmap;
		this->withDirectionalLight = withDirectionalLight;
		this->withFog = withFog;
		this->pointLightCount = pointLightCount;
	}
	BOOL32 equals(const AssimpShaderConfig &other) {
		return (hwAccel == other.hwAccel &&
				withRGBAShadowmap == other.withRGBAShadowmap &&
				withDirectionalLight == other.withDirectionalLight &&
				withFog == other.withFog &&
				pointLightCount == other.pointLightCount &&
				vertexLighting == other.vertexLighting);
	}
	BOOL32 hwAccel;
	BOOL32 withRGBAShadowmap;
	BOOL32 withDirectionalLight;
	BOOL32 withFog;
	S32 pointLightCount;
	BOOL32 vertexLighting;
};

class AssimpRenderer: public Renderer {
public:
	AssimpRenderer(GameContext *context);
	virtual ~AssimpRenderer();
	virtual void init(BOOL32 newContext);
	virtual void render(RenderItem *item, BOOL32 transparent);
    void renderShadow(RenderItem* item);
    void unbind();
        
private:
	ShaderProgram* createShadowShaderProgram(BOOL32 newContext, BOOL32 hwSkinned);
	void bindShader(ShaderProgram *shaderProgram, const Vector3f &ecLightDir, const AssimpShaderConfig &config);
	void setupPointLights(ShaderProgram *shaderProgram, RenderItem *item, Matrix4f mv, GLAssimpMeshBinding *meshBinding);
	void renderNode(RenderNode *node, GLAssimpBinding *binding, Matrix4f mv, RenderItem *item, const Vector3f &ecLightDir, const Vector3f &halfplane, BOOL32 transparent);
    
    void renderNodeShadow(RenderNode *node, GLAssimpBinding *binding, RenderItem *item, Matrix4f mv);
	void bindMaterial(RenderItem *item, GLAssimpMeshBinding *meshBinding, GLAssimpBinding *binding, const AssimpShaderConfig &config);
	void renderSkeleton(RenderItem *item);
	void RenderSkeletonNode(RenderNode* node, const Matrix4f parent);
	void drawLine(const Vector3f& from, const Vector3f& to,	const Vector3f& color);
	void getShadowShaderTag(char *buf, BOOL32 hwAccel);
	void getShaderTag(char *buf, AssimpShaderConfig config);
	ShaderProgram* addShaderProgram(const char *tag, AssimpShaderConfig config);
	GameContext *context;
	ShaderProgram *skelShaderProgram;
	ShaderProgram *currentShaderProgram;
    GLAssimpMeshBinding *currentMeshBinding;
	BOOL32 lightsBound;
	F32 vPos[SKEL_MAX_LINES * 6];
	F32 vColor[SKEL_MAX_LINES * 8];
	S32 lineCount;
};

#endif /* ASSIMPRENDERER_H_ */
