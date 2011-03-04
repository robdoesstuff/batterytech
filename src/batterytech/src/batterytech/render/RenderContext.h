/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : RenderContext.h
// Description : The current rendering context
// Usage       : Use this to simulate GLES1 matrix functionality in a GLES2 environment
//============================================================================

#ifndef RENDERCONTEXT_H_
#define RENDERCONTEXT_H_

#include "../primitives.h"
#include "../util/esTransform.h"
#include "../math/Vec4f.h"

namespace BatteryTech {

	class RenderContext {
	public:
		RenderContext();
		virtual ~RenderContext();
		/**
		 * Current projection matrix
		 */
		ESMatrix projMatrix;
		/**
		 * Current modelview matrix
		 */
		ESMatrix mvMatrix;
		/**
		 * Current color (like glColor())
		 */
		Vec4f colorFilter;
	};

}
#endif /* RENDERCONTEXT_H_ */
