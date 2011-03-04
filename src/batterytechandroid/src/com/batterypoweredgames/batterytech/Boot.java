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
// Name        : Boot.java
// Description : JNI Bindings and hooks.
//             : Do not rename this class, its package or any of its functions as they are mapped via JNI-callbacks from androidgeneral.cpp and boot.cpp
//             : You must update the JNI callbacks on the native side if you rename or move any critical methods or crashing will result
// Usage       : Implement custom ad handling, vibration and any other android-specific customizations via hooks here.
//============================================================================

package com.batterypoweredgames.batterytech;

import java.io.IOException;
import java.io.InputStream;

import com.batterypoweredgames.demoapp.VibrationManager;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

public class Boot {
	private static final String TAG = "Boot";

	private Activity activity;
	private AudioWrapper audioWrapper;
	private View view;
	private AudioBridge audioBridge;
	private VibrationManager vibrationManager;
	public boolean audioTrackStartedViaNative;

	public Boot(Activity activity, View view) {
		this.activity = activity;
		this.view = view;
		audioTrackStartedViaNative = false;
		vibrationManager = new VibrationManager(activity);
	}

	public void setAudioBridge(AudioBridge audioBridge) {
		this.audioBridge = audioBridge;
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
		if (vibrationManager != null) {
			vibrationManager.release();
		}
		vibrationManager = null;
	}

	// ----------- vibration JNI callbacks ------------
	// directs calls to app-specific vibration manager
	public void playVibrationEffect(int effectId, float intensity) {
		vibrationManager.playVibrationEffect(effectId, intensity);
	}

	public void startVibrationEffect(int effectId, float intensity) {
		vibrationManager.startVibrationEffect(effectId, intensity);
	}

	public void stopVibrationEffect(int effectId) {
		vibrationManager.stopVibrationEffect(effectId);
	}

	public void stopAllVibrationEffects() {
		vibrationManager.stopAllVibrationEffects();
	}

	// ------------ native audio management JNI callbacks -----------

	/**
	 * JNI Callback to start the audiotrack
	 * 
	 * Do not call this from anywhere but the native batterytech audiomanager
	 */
	public void startAudioTrack() {
		audioTrackStartedViaNative = true;
		audioBridge.startAudio();
	}

	/**
	 * JNI Callback to stop the audiotrack
	 */
	public void stopAudioTrack() {
		audioBridge.stopAudio();
	}

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
		activity.finish();
	}

	public void showAd() {
		// integrate with your android ad provider here
	}

	public void hideAd() {
		// integrate with your android ad provider here
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
