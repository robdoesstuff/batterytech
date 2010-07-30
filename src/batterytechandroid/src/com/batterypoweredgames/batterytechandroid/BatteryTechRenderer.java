package com.batterypoweredgames.batterytechandroid;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.batterypoweredgames.batterytech.Boot;

import android.opengl.GLSurfaceView.Renderer;

public class BatteryTechRenderer implements Renderer {

	public void onDrawFrame(GL10 gl) {
		Boot.update(0.03f);
		Boot.draw();
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		Boot.init();
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	}

	public void release() {
	}
}
