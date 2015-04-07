package com.batterypoweredgames.batterytech;

import android.opengl.GLSurfaceView;

public class GLSVContextPreserver {

	void setPreserveContextOnPause(GLSurfaceView view, boolean preserveOnPause) {
		view.setPreserveEGLContextOnPause(preserveOnPause);
	}
}
