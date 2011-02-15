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

class RenderContext {
public:
	RenderContext();
	virtual ~RenderContext();
	ESMatrix projMatrix;
	ESMatrix mvMatrix;
};

#endif /* RENDERCONTEXT_H_ */
