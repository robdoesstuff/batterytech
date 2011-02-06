package com.batterypoweredgames.demoapp;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class DemoAppView extends GLSurfaceView {

	private DemoAppRenderer renderer;

	public DemoAppView(Context context) {
		super(context);
		renderer = new DemoAppRenderer(context, this);
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
