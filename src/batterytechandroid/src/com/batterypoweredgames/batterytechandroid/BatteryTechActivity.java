package com.batterypoweredgames.batterytechandroid;

import android.app.Activity;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

public class BatteryTechActivity extends Activity {
	private BatteryTechView btechView;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
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