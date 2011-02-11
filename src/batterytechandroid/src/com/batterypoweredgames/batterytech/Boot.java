package com.batterypoweredgames.batterytech;

import java.io.IOException;
import java.io.InputStream;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
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

	private Activity activity;
	private AudioWrapper audioWrapper;
	private View view;

	static {
		System.loadLibrary("demo-app");
	}

	public Boot(Activity activity, View view) {
		this.activity = activity;
		this.view = view;
	}

	public byte[] loadAsset(String name) {
		//Log.d(TAG, "Loading Asset " + name);
		byte[] data = null;
		try {
			InputStream iStream = activity.getAssets().open(name);
			int size = iStream.available();
			data = new byte[size];
			iStream.read(data);
			iStream.close();
		} catch (IOException e) {
			Log.e(TAG, e.getMessage() + " assetname=" + name, e);
		}
		return data;
	}

	public int getAssetLength(String name) {
		int size = 0;
		try {
			InputStream iStream = activity.getAssets().open(name);
			size = iStream.available();
			iStream.close();
		} catch (IOException e) {
			Log.e(TAG, e.getMessage() + " assetname=" + name, e);
		}
		return size;
	}

	public byte[] readAssetChunk(String name, int offset, int maxBytes) {
		byte[] data = null;
		try {
			InputStream iStream = activity.getAssets().open(name, AssetManager.ACCESS_RANDOM);
			iStream.skip(offset);
			// InputStream.available() is not reliable but the android implementation seems to work correctly.
			int size = iStream.available();
			if (size > maxBytes) {
				size = maxBytes;
			}
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
		this.activity = null;
		this.view = null;
		if (this.audioWrapper != null) {
			releaseAudioManagement();
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

	// native audio management JNI callbacks

	public void initAudioManagement(int streams) {
		audioWrapper = new AudioWrapper(activity);
		audioWrapper.init(streams);
	}

	public void releaseAudioManagement() {
		if (audioWrapper != null) {
			audioWrapper.release();
		}
		audioWrapper = null;
	}

	public void loadSound(String assetName) {
		if (audioWrapper != null) {
			audioWrapper.loadSound(assetName);
		}
	}

	public int playSound(String assetName, float leftVol, float rightVol, int loops, float rate) {
		if (audioWrapper != null) {
			return audioWrapper.playSound(assetName, leftVol, rightVol, loops, rate);
		}
		return -1;
	}

	public void stopSound(int streamId) {
		if (audioWrapper != null) {
			audioWrapper.stopSound(streamId);
		}
	}

	public void stopSound(String assetName) {
		if (audioWrapper != null) {
			audioWrapper.stopSound(assetName);
		}
	}

	public void stopAllSounds() {
		if (audioWrapper != null) {
			audioWrapper.stopAllSounds();
		}
	}

	public void unloadSound(String assetName) {
		if (audioWrapper != null) {
			audioWrapper.unloadSound(assetName);
		}
	}
	
	public void setSoundLoops(int streamId, int loops) {
		if (audioWrapper != null) {
			audioWrapper.setLoops(streamId, loops);
		}
	}
	
	public void setSoundVolume(int streamId, float leftVol, float rightVol) {
		if (audioWrapper != null) {
			audioWrapper.setVolume(streamId, leftVol, rightVol);
		}
	}
	
	public void setSoundRate(int streamId, float rate) {
		if (audioWrapper != null) {
			audioWrapper.setRate(streamId, rate);
		}
	}
	
	public int playStreamingSound(String assetName, float leftVol, float rightVol, int loops, float rate) {
		if (audioWrapper != null) {
			return audioWrapper.playStreamingSound(assetName, leftVol, rightVol, loops, rate);
		}
		return -1;
	}
	
	public void stopStreamingSound(String assetName) {
		if (audioWrapper != null) {
			audioWrapper.stopStreamingSound(assetName);
		}
	}

	public void showKeyboard() {
		//Log.d(TAG, "Showing Keyboard");
		InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_IMPLICIT_ONLY);
	} 
	
	public void hideKeyboard() {
		//Log.d(TAG, "Hiding Keyboard");
		//InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
		InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.hideSoftInputFromWindow(view.getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
		//imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_IMPLICIT_ONLY); 
	}

	public void exit() {
		//Log.d(TAG, "Hiding Keyboard");
		activity.finish();
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
