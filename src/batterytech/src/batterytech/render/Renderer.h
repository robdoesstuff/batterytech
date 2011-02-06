/*
 * Renderer.h
 *
 *  Created on: Aug 27, 2010
 *      Author: rgreen
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "../platform/platformgl.h"
#include "../primitives.h"


class Renderer {
public:
	virtual void init(BOOL32 newContext) = 0;
	virtual ~Renderer();
protected:
	virtual GLuint loadTexture(const char *name);
};

#endif /* RENDERER_H_ */
