package com.batterypoweredgames.batterytechandroid;

import android.app.Activity;
import android.os.Bundle;

public class BatteryTechActivity extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(new BatteryTechView(this));
    }
}