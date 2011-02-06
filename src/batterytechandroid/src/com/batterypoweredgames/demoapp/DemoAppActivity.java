package com.batterypoweredgames.demoapp;


import java.util.concurrent.ArrayBlockingQueue;

import android.app.Activity;
import android.hardware.SensorManager;
import android.media.AudioManager;
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
        btechView = new DemoAppView(this);
        setContentView(btechView);
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
		if (btechView != null) {
			if (btechView.getRenderer() != null) {
				// handle specials
				int key = event.getUnicodeChar();
				if (keyCode == KeyEvent.KEYCODE_DEL) {
					key = 8;
				}
				btechView.getRenderer().keyDown(key);
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (btechView != null) {
			if (btechView.getRenderer() != null) {
				int key = event.getUnicodeChar();
				if (keyCode == KeyEvent.KEYCODE_DEL) {
					key = 8;
				}
				btechView.getRenderer().keyUp(key);
			}
		}
		return super.onKeyUp(keyCode, event);
	}
}