/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : DemoAppView16.java
// Description : BatteryTech's view for Android 1.6 - only capable of OpenGL ES 1.0/1.1
//============================================================================

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
