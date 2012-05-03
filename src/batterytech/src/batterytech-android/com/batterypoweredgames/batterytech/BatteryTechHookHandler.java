package com.batterypoweredgames.batterytech;

public interface BatteryTechHookHandler {

	public void showAd();

	public void hideAd();
	
	public String hook(String hook);
}
