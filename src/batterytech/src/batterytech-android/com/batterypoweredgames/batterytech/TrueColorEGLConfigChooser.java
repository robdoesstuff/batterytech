/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : TrueColorEGLConfigChooser.java
// Description : Attempts to use 32 bit color, falls back to 16 bit when not available
//============================================================================
package com.batterypoweredgames.batterytech;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView.EGLConfigChooser;
import android.view.SurfaceHolder;

public class TrueColorEGLConfigChooser implements EGLConfigChooser {

	protected int[] mValue = new int[1];
	protected int[] mConfigSpec32;
	protected int[] mConfigSpec16;
	private int mRedSize, mBlueSize, mGreenSize, mAlphaSize;
	private int stencil;
	private int depth;
	
	/**
	 * Uses holder to turn translucency on and off for 32/16 bit
	 * @param holder
	 */
	public TrueColorEGLConfigChooser(int depth, int stencil) {
		this.depth = depth;
		this.stencil = stencil;
		mConfigSpec32 = new int[] { EGL10.EGL_RED_SIZE, 8, EGL10.EGL_GREEN_SIZE, 8, EGL10.EGL_BLUE_SIZE,
				8, EGL10.EGL_ALPHA_SIZE, 8, EGL10.EGL_DEPTH_SIZE, depth, EGL10.EGL_STENCIL_SIZE,
				stencil, EGL10.EGL_NONE };
		mConfigSpec16 = new int[] { EGL10.EGL_RED_SIZE, 5, EGL10.EGL_GREEN_SIZE, 6, EGL10.EGL_BLUE_SIZE,
				5, EGL10.EGL_ALPHA_SIZE, 0, EGL10.EGL_DEPTH_SIZE, depth, EGL10.EGL_STENCIL_SIZE,
				stencil, EGL10.EGL_NONE };
	}
	
	public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
		EGLConfig config = chooseConfig(egl, display, true);
		if (config == null) {
			// revert to 16 bit
			config = chooseConfig(egl, display, false);
			if (config == null) {
				throw new IllegalArgumentException("No config chosen");
			}
		}
		return config;
	}

	public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, boolean try32bit) {
		int[] configSpec;
		if (try32bit) {
			mRedSize = mBlueSize = mGreenSize = mAlphaSize = 8;
			configSpec = mConfigSpec32;
		} else {
			mRedSize = 5;
			mBlueSize = 6;
			mGreenSize = 5;
			mAlphaSize = 0;
			configSpec = mConfigSpec16;
		}
		int[] num_config = new int[1];
		egl.eglChooseConfig(display, configSpec, null, 0, num_config);
		int numConfigs = num_config[0];
		if (numConfigs <= 0) {
			throw new IllegalArgumentException("No configs match configSpec");
		}
		EGLConfig[] configs = new EGLConfig[numConfigs];
		egl.eglChooseConfig(display, configSpec, configs, numConfigs, num_config);
		EGLConfig config = chooseConfig(egl, display, configs);
		return config;
	}
	
	public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
		EGLConfig closestConfig = null;
		int closestDistance = 1000;
		for (EGLConfig config : configs) {
			int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
			int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
			if (d >= depth && s >= stencil) {
				int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);
				int distance = Math.abs(r - mRedSize) + Math.abs(g - mGreenSize) + Math.abs(b - mBlueSize)
						+ Math.abs(a - mAlphaSize);
				if (distance < closestDistance) {
					closestDistance = distance;
					closestConfig = config;
				}
			}
		}
		return closestConfig;
	}

	private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute,
			int defaultValue) {
		if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
			return mValue[0];
		}
		return defaultValue;
	}

}
