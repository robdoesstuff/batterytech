/*
 * RenderContext.cpp
 *
 *  Created on: Feb 14, 2011
 *      Author: rgreen
 */

#include "RenderContext.h"

RenderContext::RenderContext() {
	esMatrixLoadIdentity(&projMatrix);
	esMatrixLoadIdentity(&mvMatrix);
}

RenderContext::~RenderContext() {
}
