package com.batterypoweredgames.luaengine;

import tv.ouya.console.api.OuyaController;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import com.batterypoweredgames.batterytech.BatteryTechActivity;
import com.batterypoweredgames.batterytech.GLSettings;

public class MyActivity extends BatteryTechActivity {

	private static final String TAG = "MyActivity";
	
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
	
	private void sendBTKeyDown(int keyChar, int keyCode) {
		 if (btechView != null) {
				if (btechView.getRenderer() != null) {
					btechView.getRenderer().keyDown(keyChar, keyCode);
				}
		 }
	}
	
	private void sendBTKeyUp(int keyChar, int keyCode) {
		 if (btechView != null) {
				if (btechView.getRenderer() != null) {
					btechView.getRenderer().keyUp(keyChar, keyCode);
				}
		 }
	}
	
	private int getOuyaKeyChar(int playerNumber, int keyCode) {
		int playerShift = playerNumber * 16;
		// player1 range = 96-111, player2 = 112-127, player3 = 128-143, player4 = 144-159
	    switch(keyCode){
	    	// 96,97,98,99,100,101,102,103,104,105,106,107
	        case OuyaController.BUTTON_O:
	        case OuyaController.BUTTON_U:
	        case OuyaController.BUTTON_Y:
	        case OuyaController.BUTTON_A:
	        case OuyaController.BUTTON_L1:
	        case OuyaController.BUTTON_L2:
	        case OuyaController.BUTTON_L3:
	        case OuyaController.BUTTON_R1:
	        case OuyaController.BUTTON_R2:
	        case OuyaController.BUTTON_R3:
        	return keyCode + playerShift;
            // 19,20,21,22 -> remap to 108,109,110,111
	        case OuyaController.BUTTON_DPAD_DOWN:
	        case OuyaController.BUTTON_DPAD_UP:
	        case OuyaController.BUTTON_DPAD_LEFT:
	        case OuyaController.BUTTON_DPAD_RIGHT:
	        return (keyCode + 99) + playerShift;
	    }
	    return 0;
	}
	
	@Override
	public boolean onKeyDown(final int keyCode, KeyEvent event){
	    //Get the player #
	    int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
	    // Log.d(TAG, "Device ID = " + event.getDeviceId() + ", player = " + player + " keyCode=" + keyCode);
	    boolean handled = false;
	    //Handle the input
	    switch(keyCode){
	        case OuyaController.BUTTON_O:
	        case OuyaController.BUTTON_U:
	        case OuyaController.BUTTON_Y:
	        case OuyaController.BUTTON_A:
	        case OuyaController.BUTTON_DPAD_DOWN:
	        case OuyaController.BUTTON_DPAD_UP:
	        case OuyaController.BUTTON_DPAD_LEFT:
	        case OuyaController.BUTTON_DPAD_RIGHT:
	        case OuyaController.BUTTON_L1:
	        case OuyaController.BUTTON_L2:
	        case OuyaController.BUTTON_L3:
	        case OuyaController.BUTTON_R1:
	        case OuyaController.BUTTON_R2:
	        case OuyaController.BUTTON_R3:
	        	sendBTKeyDown(getOuyaKeyChar(player, keyCode), keyCode);
	            handled = true;
	            break;
	    }
	    return handled || super.onKeyDown(keyCode, event);
	}
	
	@Override
	public boolean onKeyUp(final int keyCode, KeyEvent event){
	    //Get the player #
	    int player = OuyaController.getPlayerNumByDeviceId(event.getDeviceId());
	    // Log.d(TAG, "Device ID = " + event.getDeviceId() + ", player = " + player + " keyCode=" + keyCode);
	    boolean handled = false;

	    //Handle the input
	    switch(keyCode){
	        case OuyaController.BUTTON_O:
	        case OuyaController.BUTTON_U:
	        case OuyaController.BUTTON_Y:
	        case OuyaController.BUTTON_A:
	        case OuyaController.BUTTON_DPAD_DOWN:
	        case OuyaController.BUTTON_DPAD_UP:
	        case OuyaController.BUTTON_DPAD_LEFT:
	        case OuyaController.BUTTON_DPAD_RIGHT:
	        case OuyaController.BUTTON_L1:
	        case OuyaController.BUTTON_L2:
	        case OuyaController.BUTTON_L3:
	        case OuyaController.BUTTON_R1:
	        case OuyaController.BUTTON_R2:
	        case OuyaController.BUTTON_R3:
	        	sendBTKeyUp(getOuyaKeyChar(player, keyCode), keyCode);
	            handled = true;
	            break;
	    }
	    return handled || super.onKeyDown(keyCode, event);
	}
	
	public boolean onGenericMotionEvent(MotionEvent event) {
	    boolean handled = OuyaController.onGenericMotionEvent(event);
	    return handled || super.onGenericMotionEvent(event);
	}
	
	@Override
	public String hook(String hookData) {
		// this may generate lots of string garbage - remedy with pre-allocation and buffered conversions
		if (hookData.startsWith("getAxisValue")) {
			// Axis IDs: Left X/Y = 0/1, Right X/Y = 11/14, L2/R2 = 17/18
			String playerNumString = hookData.substring(13, 14);
			String axisIDString = hookData.substring(15, hookData.length());
			// Log.d(TAG, "getAxisValue [" + playerNumString + "][" + axisIDString + "]");
			OuyaController controller = OuyaController.getControllerByPlayer(Integer.parseInt(playerNumString));
			if (controller != null) {
				float value = controller.getAxisValue(Integer.parseInt(axisIDString));
				// shortcut the epsilon case
				if (value > -0.0001f && value < 0.0001f) {
					return "0.0";
				} 
				return Float.toString(value);
			}
			return "0.0";
		}
		return ""; 
	}
}
