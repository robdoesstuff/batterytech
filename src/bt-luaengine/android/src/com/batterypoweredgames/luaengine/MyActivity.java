package com.batterypoweredgames.luaengine;

import com.batterypoweredgames.batterytech.BatteryTechActivity;
import com.batterypoweredgames.batterytech.GLSettings;

public class MyActivity extends BatteryTechActivity {

	@Override
	public GLSettings createGLSettings() {
		// we need to use only GLES 2.0 for this application
		return new GLSettings() {
			@Override
			public boolean supportGLES2() { 
				return true;
			} 
		};
	}

}
