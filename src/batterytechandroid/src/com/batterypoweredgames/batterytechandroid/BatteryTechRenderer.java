package com.batterypoweredgames.batterytechandroid;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView.Renderer;

import com.batterypoweredgames.batterytech.Boot;

public class BatteryTechRenderer implements Renderer {

	private Context context;
	private Boot boot;
	
	public BatteryTechRenderer(Context context) {
		this.context = context;
		this.boot = new Boot(context);
	}
	
	public void onDrawFrame(GL10 gl) {
		boot.update(0.03f);
		boot.draw();
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		boot.init();
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	}

	public void release() {
		this.context = null;
		if (this.boot != null) {
			this.boot.release();
		}
		this.boot = null;
	}
}
