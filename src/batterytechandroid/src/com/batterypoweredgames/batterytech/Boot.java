package com.batterypoweredgames.batterytech;

public class Boot {

	public static native void init();
	public static native void update(float delta);
	public static native void draw();
	
	static { 
		System.loadLibrary("batterytech");
	}

}
