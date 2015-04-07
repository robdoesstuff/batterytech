package com.batterypoweredgames.batterytech;

public interface VibrationManager {

	public abstract void playVibrationEffect(int effectId, float intensity);

	public abstract void startVibrationEffect(int effectId, float intensity);

	public abstract void stopVibrationEffect(int effectId);

	public abstract void stopAllVibrationEffects();

	public abstract void release();
}