package com.batterypoweredgames.demoapp;

public interface DemoAppView {

	public abstract DemoAppRenderer getRenderer();

	public abstract void onPause();

	public abstract void onResume();

	public abstract void release();

}