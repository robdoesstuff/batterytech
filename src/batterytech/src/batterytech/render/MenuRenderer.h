/*
 * MenuRenderer.h
 *
 *  Created on: Oct 7, 2010
 *      Author: rgreen
 */

#ifndef MENURENDERER_H_
#define MENURENDERER_H_

#include "GraphicsConfiguration.h"
#include "../primitives.h"
#include "../platform/platformgl.h"
#include "../ui/UIManager.h"


class MenuRenderer {
public:
	MenuRenderer(GraphicsConfiguration *gConfig);
	S32 addTextureAsset(const char *asset);
	void render(UIManager *uiManager);
	virtual ~MenuRenderer();
private:
	GraphicsConfiguration *gConfig;
	const char **assetNames;
	GLuint *textureIds;
	BOOL32 texturesLoaded;
};

#endif /* MENURENDERER_H_ */
