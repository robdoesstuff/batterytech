package com.batterypoweredgames.demoapp;

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

import com.batterypoweredgames.batterytech.AudioBridge;
import com.batterypoweredgames.batterytech.Boot;
import com.batterypoweredgames.input.InputHandler;
import com.batterypoweredgames.input.InputObject;

public class DemoAppRenderer implements Renderer, InputHandler, SensorEventListener {
	private static final String TAG = "DemoAppRenderer";

	// TODO - query native to see if it needs us to start/stop audiotrack for pcmaudiomanager service
	
	private Activity activity;
	private Boot boot;
	private AudioBridge audioBridge;
	private long lastTickMs;
	private Object tickMutex = new Object();
	private View view;
	private boolean bootInitialized = false;
	private SensorManager sensorMgr;

	public DemoAppRenderer(Activity activity, View view) {
		this.activity = activity;
		this.view = view;
		this.boot = new Boot(activity, view);
		audioBridge = new AudioBridge(boot);
		boot.setAudioBridge(audioBridge);
	}
	
	public Boot getBoot() {
		return boot;
	}
	
	public void onDrawFrame(GL10 gl) {
		synchronized (tickMutex) {
			long curTickMs = System.nanoTime() / 1000000;
			if (lastTickMs == 0) {
				lastTickMs = curTickMs;
			}
			boot.update((curTickMs - lastTickMs) / 1000f);
			//boot.update(0.018f);
			boot.draw();
			lastTickMs = curTickMs;
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
	
	public void keyDown(int keyCode) {
		synchronized (tickMutex) {
			boot.keyDown(keyCode);
			boot.keyPressed(keyCode);
		}		
	}
	
	public void keyUp(int keyCode) {
		synchronized (tickMutex) {
			boot.keyUp(keyCode);
		}		
	}
	
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		synchronized (tickMutex) {
			if (!bootInitialized) {
				Log.i(TAG, "Calling boot.init()");
				boot.init(width, height);
				bootInitialized = true;
			}
		}
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	}

	public void onPause() {
		synchronized (tickMutex) {		
			boot.suspend();
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
		synchronized (tickMutex) {		
			boot.resume();
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
				this.boot.releaseBoot();
			}
			this.boot = null;
		}
		this.view = null;
		Log.d(TAG, "Release Complete");
	}

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
	}

	public void onSensorChanged(SensorEvent event) {
		synchronized(tickMutex) {
			if (boot != null && bootInitialized) {
				boot.accelerometerChanged(event.values[0], event.values[1], event.values[2]);
			}
		}
	}
}
