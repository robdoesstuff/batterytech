/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : InputObject.java
// Description : Encapsulates touch, keyboard and trackball user input
//============================================================================

package com.batterypoweredgames.input;

import java.util.concurrent.ArrayBlockingQueue;

import android.hardware.SensorEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class InputObject {
	public static final int TRACKBALL_MULT = 21;
	
	public static final byte EVENT_TYPE_KEY = 1;
	public static final byte EVENT_TYPE_TOUCH = 2;
	public static final byte EVENT_TYPE_TRACKBALL = 3;
	public static final byte EVENT_TYPE_ACCELEROMETER = 4;
	
	public static final int ACTION_KEY_DOWN = 1;
	public static final int ACTION_KEY_UP = 2;
	public static final int ACTION_TOUCH_DOWN = 3;
	public static final int ACTION_TOUCH_MOVE = 4;
	public static final int ACTION_TOUCH_UP = 5;
	public static final int ACTION_TRACK_MOVE = 4;

	public ArrayBlockingQueue<InputObject> pool;
	public byte eventType;
	public long time;
	public int action;
	public int keyCode;
	public int x;
	public int y;
	public int pointerId;
	public float[] values;

	public InputObject(ArrayBlockingQueue<InputObject> pool) {
		this.pool = pool;
		values = new float[3];
	}
	
	public void useEvent(KeyEvent event) {
		eventType = EVENT_TYPE_KEY;
		int a = event.getAction();
		switch (a) {
		case KeyEvent.ACTION_DOWN:
			action = ACTION_KEY_DOWN;
			break;
		case KeyEvent.ACTION_UP:
			action = ACTION_KEY_UP;
			break;
		default:
			action = 0;
		}
		time = event.getEventTime();
		keyCode = event.getKeyCode();
	}
	
	public void useTrackballEvent(MotionEvent event) {
		eventType = EVENT_TYPE_TRACKBALL;
		action = ACTION_TRACK_MOVE;
		time = event.getEventTime();
		x = (int) (event.getX() * TRACKBALL_MULT);
		y = (int) (event.getY() * TRACKBALL_MULT);
	}
	
	public void useTrackballEventHistory(MotionEvent event, int which) {
		eventType = EVENT_TYPE_TRACKBALL;
		action = ACTION_TRACK_MOVE;
		time = event.getEventTime();
		x = (int) (event.getHistoricalX(which) * TRACKBALL_MULT);
		y = (int) (event.getHistoricalY(which) * TRACKBALL_MULT);
	}

	public void useEvent(SensorEvent event) {
		eventType = EVENT_TYPE_ACCELEROMETER;
		float[] myValues = this.values;
		float[] values = event.values;
		myValues[0] = values[0];
		myValues[1] = values[1];
		myValues[2] = values[2];
		time = System.currentTimeMillis();
	}
	
	public void returnToPool() {
		pool.add(this);
	}
}
