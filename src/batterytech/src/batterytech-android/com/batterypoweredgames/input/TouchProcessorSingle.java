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
// Name        : TouchProcessorMulti.java
// Description : Converts single-touch input into touch objects - for Android 1.6 and earlier
//============================================================================

package com.batterypoweredgames.input;

import java.util.concurrent.ArrayBlockingQueue;

import android.view.MotionEvent;

import com.batterypoweredgames.batterytech.Boot;

/**
 * For Android 1.6-
 * @author rgreen
 *
 */
public class TouchProcessorSingle implements TouchProcessor {

	public TouchProcessorSingle() {
	}
	
	public void processTouchEvent(MotionEvent event, ArrayBlockingQueue<InputObject> inputObjectPool,
			InputHandler boot) {
		try {
			// history first
			int hist = event.getHistorySize();
			if (hist > 0) {
				// add from oldest to newest
				for (int i = 0; i < hist; i++) {
					InputObject input = inputObjectPool.take();
					useEventHistory(input, event, i);
					boot.feedInput(input);
				}
			}
			// current last
			InputObject input = inputObjectPool.take();
			useEvent(input, event);
			boot.feedInput(input);
		} catch (InterruptedException e) {
		}
	}

	public void useEvent(InputObject inputObject, MotionEvent event) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		int a = event.getAction();
		switch (a) {
		case MotionEvent.ACTION_DOWN:
			inputObject.action = InputObject.ACTION_TOUCH_DOWN;
			break;
		case MotionEvent.ACTION_MOVE:
			inputObject.action = InputObject.ACTION_TOUCH_MOVE;
			break;
		case MotionEvent.ACTION_UP:
			inputObject.action = InputObject.ACTION_TOUCH_UP;
			break;
		default:
			inputObject.action = 0;
		}
		inputObject.time = event.getEventTime();
		inputObject.x = (int) event.getX();
		inputObject.y = (int) event.getY();
	}
	
	public void useEventHistory(InputObject inputObject, MotionEvent event, int historyItem) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		inputObject.action = InputObject.ACTION_TOUCH_MOVE;
		inputObject.time = event.getHistoricalEventTime(historyItem);
		inputObject.x = (int) event.getHistoricalX(historyItem);
		inputObject.y = (int) event.getHistoricalY(historyItem);
	}

}
