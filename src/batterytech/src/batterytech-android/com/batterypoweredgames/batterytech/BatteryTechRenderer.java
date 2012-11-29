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
// Name        : DemoAppRenderer.java
// Description : The BatteryTech renderer for Android
//============================================================================

package com.batterypoweredgames.batterytech;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;

import com.batterypoweredgames.batterytech.AudioBridge;
import com.batterypoweredgames.batterytech.Boot;
import com.batterypoweredgames.input.InputHandler;
import com.batterypoweredgames.input.InputObject;

public class BatteryTechRenderer implements Renderer, InputHandler, SensorEventListener {
	private static final String TAG = "BatteryTechRenderer";
	
	static {
		System.loadLibrary(BatteryTechActivity.APP_LIB_NAME);
	}

	private Activity activity;
	private Boot boot;
	private AudioBridge audioBridge;
	private long lastTickMs;
	private Object tickMutex = new Object();
	private View view;
	private boolean bootInitialized = false;
	private SensorManager sensorMgr;
	private boolean usingGLES2 = false;
	int screenRotation;
	private GL10 lastGL;

	public BatteryTechRenderer(BatteryTechActivity activity, View view, boolean usingGLES2) {
		this.activity = activity;
		this.view = view;
		this.usingGLES2 = usingGLES2;
		this.boot = new Boot(activity, view);
		audioBridge = new AudioBridge(boot);
		boot.setAudioBridge(audioBridge);
	}
	
	public Boot getBoot() {
		return boot;
	}
	
	public void onDrawFrame(GL10 gl) {
		float tickDelta;
		synchronized (tickMutex) {
			long curTickMs = System.nanoTime() / 1000000;
			if (lastTickMs == 0) {
				lastTickMs = curTickMs;
			}
			tickDelta = (curTickMs - lastTickMs) / 1000f;
			boot.update(tickDelta);
			boot.draw();
			lastTickMs = curTickMs;
		}
		// give the UI thead a chance to get in here and do stuff
		Thread.yield();
		// some devices (like G2x) run faster than vsync so we can throttle it back a little here and save on battery as well as allow
		// for better opportunity for UI thread to get in
		if (tickDelta < 0.01666f) {
			long remainder = (long)((0.01666f - tickDelta) * 1000);
			// now clamp remainder to 0-16
			if (remainder < 0) {
				remainder = 0;
			}
			if (remainder > 16) {
				remainder = 16;
			}
			try {
				Thread.sleep(remainder);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

	public void feedInput(InputObject inputObject) {
		synchronized (tickMutex) {
			if (inputObject.eventType == InputObject.EVENT_TYPE_TOUCH) {
				if (inputObject.action == InputObject.ACTION_TOUCH_DOWN || inputObject.action == InputObject.ACTION_TOUCH_MOVE) {
					boot.setPointerState(inputObject.pointerId, true, inputObject.x, inputObject.y);
				} else {
					boot.setPointerState(inputObject.pointerId, false, 0, 0);
				}
			}
			inputObject.returnToPool();
		}
	}
	
	public void keyDown(int keyChar, int keyCode) {
		synchronized (tickMutex) {
			boot.keyDown(keyChar, keyCode);
			boot.keyPressed(keyChar, keyCode);
		}		
	}
	
	public void keyUp(int keyChar, int keyCode) {
		synchronized (tickMutex) {
			boot.keyUp(keyChar, keyCode);
		}		
	}
	
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		synchronized (tickMutex) {
			if (!bootInitialized) {
				Log.i(TAG, "Calling boot.init()");
				boot.init(width, height, usingGLES2);
				bootInitialized = true;
			} else {
				boot.setScreenSize(width, height);
			}
			if (lastGL != null) {
				if (lastGL != gl) {
					boot.setGraphicsContextLost(true);
				}
			}
			lastGL = gl;
		}
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	}

	public void onPause() {
		synchronized (tickMutex) {		
			boot.onSuspend();
		}
		if (sensorMgr != null) {
			sensorMgr.unregisterListener(this);
			sensorMgr = null;
		}
		//vibrationManager.onPause();
		// only stop audio if it was started via the native side
		if (boot.audioTrackStartedViaNative) {
			audioBridge.stopAudio();
		}
	}
	
	public void onResume() {
		WindowManager windowMgr = (WindowManager)activity.getSystemService(Activity.WINDOW_SERVICE);
		// getOrientation() is deprecated in Android 8 but is the same as getRotation() which is the rotation from the natural orientation of the device
		screenRotation = windowMgr.getDefaultDisplay().getOrientation();
		synchronized (tickMutex) {		
			boot.onResume();
		}
		sensorMgr = (SensorManager) activity.getSystemService(Context.SENSOR_SERVICE);
		Sensor sensor = sensorMgr.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		try {
			sensorMgr.registerListener(this, sensor, SensorManager.SENSOR_DELAY_GAME);
		} catch (UnsupportedOperationException e) {
			Log.i(TAG, "Accelerometer is not available on this device.");
		}
		//vibrationManager.onResume();
		// only resume audio if it was started via the native side
		if (boot.audioTrackStartedViaNative) {
			audioBridge.startAudio();
		}
	}
	
	/**
	 * Calls back from asynchronous function into game engine
	 * 
	 * Blocks until callback slot is free
	 * 
	 * @param callbackData The string to pass back to engine
	 */
	public void callback(String callbackData) {
		boolean success = false;
		while (!success) {
			synchronized (tickMutex) {
				success = boot.callback(callbackData);
			}
			if (!success) {
				try {
					Thread.sleep(33);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
	}

	public void release() {
		Log.d(TAG, "Releasing");
		this.activity = null;
		if (audioBridge != null) {
			audioBridge.stopAudio();
			audioBridge.release();
		}
		audioBridge = null;
		Log.d(TAG, "Releasing Batterytech Bootstrap");
		synchronized (tickMutex) {		
			if (this.boot != null) {
				this.boot.releaseBoot(bootInitialized);
			}
			this.boot = null;
			bootInitialized = false;
		}
		this.view = null;
		Log.d(TAG, "Release Complete");
	}

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	public void onSensorChanged(SensorEvent event) {
		synchronized(tickMutex) {
			if (boot != null && bootInitialized) {
				final int[] as = ACCELEROMETER_AXIS_SWAP[screenRotation];
				float screenX = (float)as[0] * event.values[as[2]];
				float screenY = (float)as[1] * event.values[as[3]];
				float screenZ = event.values[2];
				boot.accelerometerChanged(screenX, screenY * -1, screenZ);
			}
		}
	}
	
	// used to swap axis of accelerometer data based on screen rotation
	static final int ACCELEROMETER_AXIS_SWAP[][] = {
			{1, -1, 0, 1}, // ROTATION_0
			{-1, -1, 1, 0}, // ROTATION_90
			{-1, 1, 0, 1}, // ROTATION_180
			{1, 1, 1, 0}}; // ROTATION_270

}
