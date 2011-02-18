package com.batterypoweredgames.demoapp;

import android.app.Activity;
import android.opengl.GLSurfaceView;

/**
 * Android 1.5/1.6 App View
 * 
 * Only supports GLES 1.0/1.1
 * 
 * @author rgreen
 *
 */public class DemoAppView16 extends GLSurfaceView implements DemoAppView {
	private DemoAppRenderer renderer;

	static {
		System.loadLibrary(DemoAppActivity.APP_GL1_LIB_NAME);
	}

	public DemoAppView16(Activity activity) {
		super(activity);
		renderer = new DemoAppRenderer(activity, this);
		setRenderer(renderer);
		setRenderMode(RENDERMODE_CONTINUOUSLY);
	}
	
	public DemoAppRenderer getRenderer() {
		return renderer;
	}
	
	@Override
	public void onPause() {
		super.onPause();
		if (renderer != null) {
			renderer.onPause();
		}
	}

	@Override
	public void onResume() {
		super.onResume();
		if (renderer != null) {
			renderer.onResume();
		}
	}

	public void release() {
		if (renderer != null) {
			renderer.release();
		}
		renderer = null;
	}
}
