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
// Name        : DemoAppActivity.java
// Description : The primary Android activity for BatteryTech
//============================================================================

package com.batterypoweredgames.demoapp;

import java.util.concurrent.ArrayBlockingQueue;

import android.app.Activity;
import android.hardware.SensorManager;
import android.media.AudioManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

import com.batterypoweredgames.batterytech.Boot;
import com.batterypoweredgames.input.InputHandler;
import com.batterypoweredgames.input.InputObject;
import com.batterypoweredgames.input.TouchProcessor;

public class DemoAppActivity extends Activity {
	private static final String TAG = "BatteryTechActivity";
	
	public static final String APP_LIB_NAME = "demo-app";
	
	private DemoAppView btechView;
	
	private static final int INPUT_QUEUE_SIZE = 50;

	private TouchProcessor touchProcessor;
	private boolean hasMultitouch = false;
	private ArrayBlockingQueue<InputObject> inputObjectPool;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) { 
        super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
		int sdkVersion = new Integer(android.os.Build.VERSION.SDK);
		if (sdkVersion < 5) {
			btechView = new DemoAppView16(this);
		} else {
			btechView = new DemoAppView20(this);
		}
        setContentView((GLSurfaceView)btechView);
		createTouchProcessor();
		createInputObjectPool();
    }

	private void createTouchProcessor() {
		ClassLoader classLoader = DemoAppActivity.class.getClassLoader();
		
		int sdkVersion = new Integer(android.os.Build.VERSION.SDK);
		if (sdkVersion < 5) {
			hasMultitouch = false;
			//use 1.6 loader
			try {
		        Class<?> aClass = classLoader.loadClass("com.batterypoweredgames.input.TouchProcessorSingle");
		        touchProcessor = (TouchProcessor) aClass.newInstance();
		    } catch (Exception e) {
		        Log.e(TAG, e.getMessage(), e);
		        touchProcessor = null;
		    }
		} else {
			hasMultitouch = true;
			//use 2.0 loader
			try {
		        Class<?> aClass = classLoader.loadClass("com.batterypoweredgames.input.TouchProcessorMulti");
		        touchProcessor =  (TouchProcessor) aClass.newInstance();
		    } catch (Exception e) {
		        Log.e(TAG, e.getMessage(), e);
		        touchProcessor = null;
		    }
		}
		//touchProcessor = new TouchProcessorMulti();
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		if (btechView != null) {
			InputHandler renderer = btechView.getRenderer();
			if (renderer != null) {
				touchProcessor.processTouchEvent(event, inputObjectPool, renderer);
				// don't allow more than 35 motion events per second
				try {
					Thread.sleep(28);
				} catch (InterruptedException e) {
				}
				return true;
			}
		}
		return false;
	}

	private void createInputObjectPool() {
		inputObjectPool = new ArrayBlockingQueue<InputObject>(INPUT_QUEUE_SIZE);
		for (int i = 0; i < INPUT_QUEUE_SIZE; i++) {
			inputObjectPool.add(new InputObject(inputObjectPool));
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		if (btechView != null) {
			btechView.onPause();
		} 
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (btechView != null) {
			btechView.onResume();
		} 
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (btechView != null) {
			btechView.release();
		} 
		btechView = null;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN){
			return false;
		}
		if (btechView != null) {
			if (btechView.getRenderer() != null) {
				// handle specials
				int keyChar = event.getUnicodeChar();
				if (keyCode == KeyEvent.KEYCODE_DEL) {
					keyChar = 8;
				}
				btechView.getRenderer().keyDown(keyChar, keyCode);
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN){
			return false;
		}
		if (btechView != null) {
			if (btechView.getRenderer() != null) {
				int keyChar = event.getUnicodeChar();
				if (keyCode == KeyEvent.KEYCODE_DEL) {
					keyChar = 8;
				}
				btechView.getRenderer().keyUp(keyChar, keyCode);
				return true;
			}
		}
		return super.onKeyUp(keyCode, event);
	}
}