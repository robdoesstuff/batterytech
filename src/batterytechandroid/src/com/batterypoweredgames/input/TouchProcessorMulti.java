/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : TouchProcessorMulti.java
// Description : Converts multi-touch input into touch objects - for Android 2.0+
//============================================================================

package com.batterypoweredgames.input;

import java.util.concurrent.ArrayBlockingQueue;

import android.util.Log;
import android.view.MotionEvent;

import com.batterypoweredgames.batterytech.Boot;

/**
 * Converts multi-touch input into touch objects - for Android 2.0+
 * 
 */
public class TouchProcessorMulti implements TouchProcessor {
	private static final String TAG = "TouchProcessorMulti";

	private int lastKnownX[] = new int[] { -100, -100, -100, -100, -100, -100, -100, -100, -100, -100 };
	private int lastKnownY[] = new int[] { -100, -100, -100, -100, -100, -100, -100, -100, -100, -100 };

	public TouchProcessorMulti() {
	}

	public void processTouchEvent(MotionEvent event, ArrayBlockingQueue<InputObject> inputObjectPool, InputHandler boot) {
		try {
			int pointerCount = event.getPointerCount();
			for (int pointerIndex = 0; pointerIndex < pointerCount; pointerIndex++) {
				int pointerId = event.getPointerId(pointerIndex);
				if (pointerId > 9) {
					// we only support 10 touch points
					continue;
				}
				int maskedIndex = (event.getAction() & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
				//Log.d(TAG, "Idx " + pointerIndex + " Id=" + pointerId + " maskedIndex=" + maskedIndex);
				if (event.getAction() != MotionEvent.ACTION_MOVE && maskedIndex != pointerIndex) {
					// if it's an up/down/cancel/out event, mask the id to see if we should process it for this touch point
					continue;
				}
				// history first
				int hist = event.getHistorySize();
				if (hist > 0) {
					// add from oldest to newest
					for (int i = 0; i < hist; i++) {
						InputObject input = inputObjectPool.take();
						input.pointerId = pointerId;
						useEventHistory(input, event, i, pointerIndex, pointerId);
						boot.feedInput(input);
					}
				}
				// current last
				InputObject input = inputObjectPool.take();
				input.pointerId = pointerId;
				useEvent(input, event, pointerIndex, pointerId);
				boot.feedInput(input);
			}
		} catch (InterruptedException e) {
		}
	}

	public void createUpEvent(InputObject inputObject, long time, int pointerId) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		inputObject.action = InputObject.ACTION_TOUCH_UP;
		inputObject.pointerId = pointerId;
		inputObject.time = time;
		inputObject.x = lastKnownX[pointerId];
		inputObject.y = lastKnownY[pointerId];
		lastKnownX[pointerId] = -100;
		lastKnownY[pointerId] = -100;
	}

	public void useEvent(InputObject inputObject, MotionEvent event, int pointerIndex, int pointerId) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		int a = event.getAction() & MotionEvent.ACTION_MASK;
		int inX = (int) event.getX(pointerIndex);
		int inY = (int) event.getY(pointerIndex);
		//Log.d(TAG, "pointerId=" + pointerId + " a" + a + " " + inX + "," + inY);
		switch (a) {
		case MotionEvent.ACTION_DOWN:
		case MotionEvent.ACTION_POINTER_DOWN:
			inputObject.action = InputObject.ACTION_TOUCH_DOWN;
			lastKnownX[pointerId] = inX;
			lastKnownY[pointerId] = inY;
			break;
		case MotionEvent.ACTION_MOVE:
			inputObject.action = InputObject.ACTION_TOUCH_MOVE;
			lastKnownX[pointerId] = inX;
			lastKnownY[pointerId] = inY;
			break;
		case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
        case MotionEvent.ACTION_OUTSIDE:
        case MotionEvent.ACTION_CANCEL:
			inputObject.action = InputObject.ACTION_TOUCH_UP;
			lastKnownX[pointerId] = -100;
			lastKnownY[pointerId] = -100;
			break;
		default:
			inputObject.action = 0;
		}
		inputObject.time = event.getEventTime();
		inputObject.x = inX;
		inputObject.y = inY;
	}

	public void useEventHistory(InputObject inputObject, MotionEvent event, int historyItem, int pointerIndex,
			int pointerId) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		inputObject.action = InputObject.ACTION_TOUCH_MOVE;
		inputObject.time = event.getHistoricalEventTime(historyItem);
		inputObject.x = (int) event.getHistoricalX(historyItem, pointerIndex);
		inputObject.y = (int) event.getHistoricalY(historyItem, pointerIndex);
	}

}
