package com.batterypoweredgames.demoapp;

import android.os.Bundle;

import com.batterypoweredgames.batterytech.BatteryTechActivity;
import com.batterypoweredgames.batterytech.BatteryTechHookHandler;

public class DemoAppActivity extends BatteryTechActivity {

	private BatteryTechHookHandler hookHandler;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	// implement a hook handler if you want to handle custom events
    }

	@Override
	public BatteryTechHookHandler getHookHandler() {
		return hookHandler;
	}
    
}
