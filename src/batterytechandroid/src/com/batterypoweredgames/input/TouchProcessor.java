package com.batterypoweredgames.input;

import java.util.concurrent.ArrayBlockingQueue;

import com.batterypoweredgames.batterytech.Boot;

import android.view.MotionEvent;

public interface TouchProcessor {
	public void processTouchEvent(MotionEvent event, ArrayBlockingQueue<InputObject> inputObjectPool, InputHandler boot);
}
