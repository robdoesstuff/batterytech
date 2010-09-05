package com.batterypoweredgames.input;

import java.util.concurrent.ArrayBlockingQueue;

import android.view.MotionEvent;

import com.batterypoweredgames.batterytech.Boot;

/**
 * For Android 2.0+
 * 
 * @author rgreen
 * 
 */
public class TouchProcessorMulti implements TouchProcessor {
	private static final String TAG = "TouchProcessorMulti";

	private static final int BAD_POINT_THRESHOLD = 50;

	private boolean pointer0Down = false;
	private boolean pointer1Down = false;
	private int lastKnownX[] = new int[] { -100, -100 };
	private int lastKnownY[] = new int[] { -100, -100 };

	public TouchProcessorMulti() {
	}

	public void processTouchEvent(MotionEvent event, ArrayBlockingQueue<InputObject> inputObjectPool, InputHandler boot) {
		try {
			int pointerCount = event.getPointerCount();
			for (int pointerIndex = 0; pointerIndex < pointerCount; pointerIndex++) {
				int pointerId = event.getPointerId(pointerIndex);
				if (pointerId > 1) {
					// ignore pointer ids beyond 0 and 1
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
			// send touch up events that were missed
			if (pointerCount == 1) {
				int pointerId = event.getPointerId(0);
				if (pointerId == 0 && pointer1Down) {
					// pointer1 is now up
					InputObject input = inputObjectPool.take();
					createUpEvent(input, event.getEventTime(), 1);
					boot.feedInput(input);
				} else if (pointerId == 1 && pointer0Down) {
					// pointer0 is now up
					InputObject input = inputObjectPool.take();
					createUpEvent(input, event.getEventTime(), 0);
					boot.feedInput(input);
				}
			} else if (pointerCount == 0) {
				if (pointer0Down) {
					// send pointer 0 up
					InputObject input = inputObjectPool.take();
					createUpEvent(input, event.getEventTime(), 0);
					boot.feedInput(input);
				}
				if (pointer1Down) {
					// send pointer 1 up
					InputObject input = inputObjectPool.take();
					createUpEvent(input, event.getEventTime(), 1);
					boot.feedInput(input);
				}
			}

		} catch (InterruptedException e) {
		}
	}

	public void createUpEvent(InputObject inputObject, long time, int pointerId) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		inputObject.action = InputObject.ACTION_TOUCH_UP;
		inputObject.pointerId = pointerId;
		inputObject.time = time;
		if (pointerId == 0) {
			pointer0Down = false;
		} else if (pointerId == 1) {
			pointer1Down = false;
		}
		inputObject.x = lastKnownX[pointerId];
		inputObject.y = lastKnownY[pointerId];
		lastKnownX[pointerId] = -100;
		lastKnownY[pointerId] = -100;
	}

	public void useEvent(InputObject inputObject, MotionEvent event, int pointerIndex, int pointerId) {
		inputObject.eventType = InputObject.EVENT_TYPE_TOUCH;
		int a = event.getAction();
		int inX = (int) event.getX(pointerIndex);
		int inY = (int) event.getY(pointerIndex);
		//Log.d(TAG, "pointerId=" + pointerId + " a" + a + " " + inX + "," + inY);
		switch (a) {
		case MotionEvent.ACTION_DOWN:
			inputObject.action = InputObject.ACTION_TOUCH_DOWN;
			/*
			if (Math.abs(lastKnownX[1 - pointerId] - inX) < BAD_POINT_THRESHOLD
					&& Math.abs(lastKnownY[1 - pointerId] - inY) < BAD_POINT_THRESHOLD) {
				inputObject.pointerId = 1 - pointerId;
				pointerId = 1 - pointerId;
			}
			*/
			if (pointerId == 0) {
				pointer0Down = true;
			} else if (pointerId == 1) {
				pointer1Down = true;
			}
			lastKnownX[pointerId] = inX;
			lastKnownY[pointerId] = inY;
			break;
		case MotionEvent.ACTION_MOVE:
			/*
			if (Math.abs(lastKnownX[1 - pointerId] - inX) < BAD_POINT_THRESHOLD
					&& Math.abs(lastKnownY[1 - pointerId] - inY) < BAD_POINT_THRESHOLD) {
				inputObject.pointerId = 1 - pointerId;
				pointerId = 1 - pointerId;
			}*/
			if (pointerId == 0 && !pointer0Down) {
				inputObject.action = InputObject.ACTION_TOUCH_DOWN;
				pointer0Down = true;
			} else if (pointerId == 1 && !pointer1Down) {
				inputObject.action = InputObject.ACTION_TOUCH_DOWN;
				pointer1Down = true;
			} else {
				inputObject.action = InputObject.ACTION_TOUCH_MOVE;
			}
			lastKnownX[pointerId] = inX;
			lastKnownY[pointerId] = inY;
			break;
		case MotionEvent.ACTION_UP:
			inputObject.action = InputObject.ACTION_TOUCH_UP;
			lastKnownX[pointerId] = -100;
			lastKnownY[pointerId] = -100;
			if (pointerId == 0) {
				pointer0Down = false;
			} else if (pointerId == 1) {
				pointer1Down = false;
			}
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
		// HACK - historical data comes with points for incorrect pointer.
		if (Math.abs(lastKnownX[1 - pointerId] - inputObject.x) < BAD_POINT_THRESHOLD
				&& Math.abs(lastKnownY[1 - pointerId] - inputObject.y) < BAD_POINT_THRESHOLD) {
			inputObject.pointerId = 1 - pointerId;
		}
	}

}
