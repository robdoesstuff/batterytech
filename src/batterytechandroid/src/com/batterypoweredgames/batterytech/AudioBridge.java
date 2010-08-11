package com.batterypoweredgames.batterytech;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class AudioBridge {
	private static final String TAG = "AudioBridge";
	
	private static final int RATE = 44100;
	private static final int BUF_SIZE = 10000;
	private Boot boot;
	private AudioTrack audioTrack;
	private boolean run = false;
	private short[] buf;
	private Thread thread;
	
	
	public AudioBridge(Boot boot) {
		this.boot = boot;
	}

	public void startAudio() {
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, RATE, AudioFormat.CHANNEL_OUT_MONO,
				AudioFormat.ENCODING_PCM_16BIT, BUF_SIZE, AudioTrack.MODE_STREAM);
		final int minBufSize = AudioTrack.getMinBufferSize(RATE, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);
		Log.d(TAG, "Using a " + minBufSize + " byte buffer");
		run = true;
		buf = new short[minBufSize / 2];
		thread = new Thread("AudioThread") {
			public void run() {
				while(run) {
					//Log.d(TAG, "Filling audio buffer from native");
					// get audio from native side (specify in bytes)
					boot.fillAudioBuffer(buf, minBufSize);
					//Log.d(TAG, "Writing audio buffer to audioTrack");
					// write audio to android track (specify in shorts)
					audioTrack.write(buf, 0, minBufSize / 2);
				}
			}
		};
		thread.start();
		audioTrack.play();
	}

	public void stopAudio() {
		if (audioTrack != null) {
			audioTrack.stop();
		}
		run = false;
		if (thread != null) {
			try {
				thread.join();
			} catch (InterruptedException e) {
			}
		}
		audioTrack = null;
		buf = null;
	}

}
