/*
 * RenderContext.h
 *
 *  Created on: Feb 14, 2011
 *      Author: rgreen
 */

#ifndef RENDERCONTEXT_H_
#define RENDERCONTEXT_H_

#include "../primitives.h"
#include "../util/esTransform.h"
#include "../math/Vec4f.h"

class RenderContext {
public:
	RenderContext();
	virtual ~RenderContext();
	ESMatrix projMatrix;
	ESMatrix mvMatrix;
	Vec4f colorFilter;
};

#endif /* RENDERCONTEXT_H_ */
