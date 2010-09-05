package com.batterypoweredgames.batterytechandroid;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView.Renderer;

import com.batterypoweredgames.batterytech.AudioBridge;
import com.batterypoweredgames.batterytech.Boot;
import com.batterypoweredgames.input.InputHandler;
import com.batterypoweredgames.input.InputObject;

public class BatteryTechRenderer implements Renderer, InputHandler {

	private Context context;
	private Boot boot;
	private AudioBridge audioBridge;
	private long lastTickMs;
	private Object tickMutex = new Object();
	
	public BatteryTechRenderer(Context context) {
		this.context = context;
		this.boot = new Boot(context);
		audioBridge = new AudioBridge(boot);
	}
	
	public void onDrawFrame(GL10 gl) {
		synchronized (tickMutex) {
			long curTickMs = System.nanoTime() / 1000000;
			if (lastTickMs == 0) {
				lastTickMs = curTickMs;
			}
			boot.update((curTickMs - lastTickMs) / 1000f);
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
	
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		synchronized (tickMutex) {		
			boot.init(width, height);
		}
		audioBridge.startAudio();
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	}

	public void release() {
		this.context = null;
		if (audioBridge != null) {
			audioBridge.stopAudio();
		}
		audioBridge = null;
		synchronized (tickMutex) {		
			if (this.boot != null) {
				this.boot.release();
			}
			this.boot = null;
		}
	}
}
