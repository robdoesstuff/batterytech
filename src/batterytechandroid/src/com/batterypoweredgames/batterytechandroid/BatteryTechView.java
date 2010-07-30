package com.batterypoweredgames.batterytechandroid;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class BatteryTechView extends GLSurfaceView {

	private BatteryTechRenderer renderer;

	public BatteryTechView(Context context) {
		super(context);
		renderer = new BatteryTechRenderer();
		setRenderer(renderer);
		setRenderMode(RENDERMODE_CONTINUOUSLY);
	}
	
	public void release() {
		if (renderer != null) {
			renderer.release();
		}
		renderer = null;
	}

}
