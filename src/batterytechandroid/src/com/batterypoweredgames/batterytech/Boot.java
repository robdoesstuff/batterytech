package com.batterypoweredgames.batterytech;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

/**
 * Do not rename this class, change its package or rename any of its methods unless you update androidgeneral.cpp to
 * match.
 * 
 * @author rgreen
 * 
 */
public class Boot {
	private static final String TAG = "Boot";

	private Context context;
	private SoundPoolWrapper soundPoolWrapper;
	private View view;

	static {
		System.loadLibrary("demo-app");
	}

	public Boot(Context context, View view) {
		this.context = context;
		this.view = view;
	}

	public byte[] loadAsset(String name) {
		Log.d(TAG, "Loading Asset " + name);
		byte[] data = null;
		try {
			InputStream iStream = context.getAssets().open(name);
			int size = iStream.available();
			data = new byte[size];
			iStream.read(data);
			iStream.close();
		} catch (IOException e) {
			Log.e(TAG, e.getMessage() + " assetname=" + name, e);
		}
		return data;
	}

	public String getExternalStorageDirPath() {
		return Environment.getExternalStorageDirectory().getAbsolutePath();
	}

	public void releaseBoot() {
		release();
		this.context = null;
		this.view = null;
		if (this.soundPoolWrapper != null) {
			releaseSoundPool();
		}
	}

	// vibration JNI callbacks

	public void playVibrationEffect(int effectId, float intensity) {
	}

	public void startVibrationEffect(int effectId, float intensity) {
	}

	public void stopVibrationEffect(int effectId) {
	}

	public void stopAllVibrationEffects() {
	}

	// soundpool JNI callbacks

	public void initSoundPool(int streams) {
		soundPoolWrapper = new SoundPoolWrapper(context);
		soundPoolWrapper.init(streams);
	}

	public void releaseSoundPool() {
		if (soundPoolWrapper != null) {
			soundPoolWrapper.release();
		}
		soundPoolWrapper = null;
	}

	public void loadSound(String assetName) {
		if (soundPoolWrapper != null) {
			soundPoolWrapper.loadSound(assetName);
		}
	}

	public int playSound(String assetName, float leftVol, float rightVol, int loops, float rate) {
		if (soundPoolWrapper != null) {
			return soundPoolWrapper.playSound(assetName, leftVol, rightVol, loops, rate);
		}
		return -1;
	}

	public void stopSound(int streamId) {
		if (soundPoolWrapper != null) {
			soundPoolWrapper.stopSound(streamId);
		}
	}

	public void stopSound(String assetName) {
		if (soundPoolWrapper != null) {
			soundPoolWrapper.stopSound(assetName);
		}
	}

	public void stopAllSounds() {
		if (soundPoolWrapper != null) {
			soundPoolWrapper.stopAllSounds();
		}
	}

	public void unloadSound(String assetName) {
		if (soundPoolWrapper != null) {
			soundPoolWrapper.unloadSound(assetName);
		}
	}
	
	public void showKeyboard() {
		Log.d(TAG, "Showing Keyboard");
		InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_IMPLICIT_ONLY);
	} 
	
	public void hideKeyboard() {
		Log.d(TAG, "Hiding Keyboard");
		//InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
		InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.hideSoftInputFromWindow(view.getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
		//imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_IMPLICIT_ONLY); 
	}

	public native void init(int width, int height);
	public native void release();
	public native void update(float delta);
	public native void draw();
	public native void suspend();
	public native void resume();
	public native void fillAudioBuffer(short[] pcmData, int count);
	public native void setPointerState(int pointerId, boolean down, int x, int y);
	public native void keyUp(int keyCode);
	public native void keyDown(int keyCode);
	public native void keyPressed(int keyCode);
	public native void accelerometerChanged(float x, float y, float z);
}
