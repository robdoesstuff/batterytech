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

//============================================================================
// Name        : RenderContext.h
// Description : The current rendering context
// Usage       : Use this to simulate GLES1 matrix functionality in a GLES2 environment
//============================================================================

#ifndef RENDERCONTEXT_H_
#define RENDERCONTEXT_H_

#include "../primitives.h"
#include "../util/esTransform.h"
#include "../math/Vector4.h"
#include "../math/Matrix4.h"
#include "../util/HashTable.h"

namespace BatteryTech {

	class RenderContext {
	public:
		RenderContext();
		virtual ~RenderContext();
		/**
		 * Current projection matrix
		 */
		Matrix4f projMatrix;
		/**
		 * Current modelview matrix
		 */
		Matrix4f mvMatrix;
		/**
		 * Current color (like glColor())
		 */
		Vector4f colorFilter;
		/**
		 * User-defined render values - specific to your application (put whatever you want in here)
		 */
		StrHashTable<void*> *userValues;
	};

}
#endif /* RENDERCONTEXT_H_ */
