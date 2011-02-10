package com.batterypoweredgames.demoapp;

import android.app.Activity;
import android.opengl.GLSurfaceView;

public class DemoAppView extends GLSurfaceView {

	private DemoAppRenderer renderer;

	public DemoAppView(Activity activity) {
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
