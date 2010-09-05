package com.batterypoweredgames.batterytech;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.util.Log;

public class Boot {
	private static final String TAG = "Boot";
	
	private Context context;
	
	public Boot(Context context) {
		this.context = context;
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
	
	public void release() {
		this.context = null;
	}
	
	public native void init(int width, int height);
	public native void update(float delta);
	public native void draw();
	public native void fillAudioBuffer(short[] pcmData, int count);
	
	static { 
		System.loadLibrary("batterytech");
	}
}
