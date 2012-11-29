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
// Name        : RenderContext.cpp
// Description : The current rendering context
// Usage       : Use this to simulate GLES1 matrix functionality in a GLES2 environment
//============================================================================

#include "RenderContext.h"

#define MAX_USER_VALUES 100

namespace BatteryTech {

	RenderContext::RenderContext() {
		userValues = new StrHashTable<void*>(MAX_USER_VALUES);
	}

	RenderContext::~RenderContext() {
		delete userValues;
	}

}
