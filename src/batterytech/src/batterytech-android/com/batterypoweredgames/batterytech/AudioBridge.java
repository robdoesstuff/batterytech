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
// Name        : AudioBridge.java
// Description : Bridges native PCM mixer with Android AudioTrack
//============================================================================

package com.batterypoweredgames.batterytech;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class AudioBridge {
	private static final String TAG = "AudioBridge";
	
	private static final int RATE = 44100;
	private Boot boot;
	private AudioTrack audioTrack;
	private boolean run = false;
	private short[] buf;
	private Thread thread;
	
	
	public AudioBridge(Boot boot) {
		this.boot = boot;
	}  

	public void startAudio() {
		final int minBufSize = AudioTrack.getMinBufferSize(RATE, AudioFormat.CHANNEL_CONFIGURATION_STEREO, AudioFormat.ENCODING_PCM_16BIT);
		Log.d(TAG, "Using a " + minBufSize + " byte buffer");
		audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, RATE, AudioFormat.CHANNEL_CONFIGURATION_STEREO,
				AudioFormat.ENCODING_PCM_16BIT, minBufSize, AudioTrack.MODE_STREAM);
		run = true;
		// 17ms updates to an 8192 byte buffer (on galaxy s) which is 46ms?
		buf = new short[768]; 
		thread = new Thread("AudioThread") {
			public void run() {
				while(run) {
					//Log.d(TAG, "Filling audio buffer from native");
					// get audio from native side (specify in bytes)
					boot.fillAudioBuffer(buf, 768 * 2); // TODO - NPE on resume
					//Log.d(TAG, "Writing audio buffer to audioTrack");
					// write audio to android track (specify in shorts)
					audioTrack.write(buf, 0, 768);
				}
			}
		};
		thread.start();
		audioTrack.play();
	}

	public void stopAudio() {
		Log.d(TAG, "Stopping Audio");
		run = false;
		if (thread != null) {
			try {
				thread.join();
			} catch (InterruptedException e) {
			}
		}
		if (audioTrack != null) {
			audioTrack.stop();
			audioTrack.release();
		}
		audioTrack = null;
		buf = null;
	}
	
	public void release() {
		boot = null;
	}
}
