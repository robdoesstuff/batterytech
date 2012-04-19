/*
 * RenderUtil.h
 *
 */

#ifndef RENDERUTIL_H_
#define RENDERUTIL_H_

class AppContext;

class RenderUtil  {
public:
	static void setupGL(AppContext *context);
	static void startGLRender(AppContext *context);
};

#endif /* RENDERUTIL_H_ */
