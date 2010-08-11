package com.batterypoweredgames.batterytechandroid;

import android.app.Activity;
import android.os.Bundle;

public class BatteryTechActivity extends Activity {
	private BatteryTechView btechView;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        btechView = new BatteryTechView(this);
        setContentView(btechView);
    }

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (btechView != null) {
			btechView.release();
		} 
		btechView = null;
	}
    
    
}