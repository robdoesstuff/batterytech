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
// Name        : DemoAppActivity.java
// Description : The primary Android activity for BatteryTech
//============================================================================

package com.batterypoweredgames.batterytech;

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

public class BatteryTechActivity extends Activity {
	private static final String TAG = "BatteryTechActivity";
	
	public static final String APP_LIB_NAME = "batterytech-app";
	
	protected BatteryTechView btechView;
	
	private static final int INPUT_QUEUE_SIZE = 50;

	private TouchProcessor touchProcessor;
	private boolean hasMultitouch = false;
	private boolean throttleTouchInput = false;
	private ArrayBlockingQueue<InputObject> inputObjectPool; 
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) { 
        super.onCreate(savedInstanceState);
        doCreate();
    }
    
    public void doCreate() {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
		int sdkVersion = new Integer(android.os.Build.VERSION.SDK);
		if (sdkVersion < 4) {
			throttleTouchInput = true;
		} else {
			throttleTouchInput = false;
		}
		if (sdkVersion < 5) {
			btechView = new BatteryTechView16(this);
		} else {
			btechView = new BatteryTechView20(this);
		}
        setContentView((GLSurfaceView)btechView);
		createTouchProcessor();
		createInputObjectPool();
    }

	protected void createTouchProcessor() {
		ClassLoader classLoader = BatteryTechActivity.class.getClassLoader();
		
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
				if (throttleTouchInput == true) {
					// don't allow more than 35 motion events per second
					try {
						Thread.sleep(28);
					} catch (InterruptedException e) {
					}
				}
				return true;
			}
		}
		return false;
	}

	protected void createInputObjectPool() {
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
	
	/**
	 * Allows a BatteryTech application to provide hooks for additional Android feature integration or 3rd party library integration
	 * Override this and implement custom hooks
	 * 
	 * @return an instance of a BatteryTechHookHandler
	 */
	public String hook(String hookData) { return ""; }
	
	/**
	 * Allows a BatteryTech application to show an ad using a third party ad library
	 * Override this and add the ad sample code.  More code may also be required to set up the ad network in onCreate or onResume
	 */
	public void showAd() {};

	/**
	 * Allows a BatteryTech application to hide an ad using a third party ad library
	 * Override this and add the ad sample code.  More code may also be required to set up the ad network in onCreate or onResume
	 */
	public void hideAd() {};

	/**
	 * Allows a BatteryTech application to provide a custom vibration manager
	 * Override this and extend the default VibrationManager to customize
	 * 
	 * @return a subclass of VibrationManager
	 */
	public VibrationManager createVibrationManager() { return new DefaultVibrationManager(this); }
	
	/**
	 * Allows a BatteryTech application to use different OpenGL ES Settings, such as 2.0.
	 * Override this and extend the default GLSettings to customize
	 * 
	 * @return a subclass of GLSettings
	 */
	public GLSettings createGLSettings() { return new GLSettings(); }
}