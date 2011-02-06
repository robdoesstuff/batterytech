package com.batterypoweredgames.batterytech;



import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.Log;

public class SoundPoolWrapper {
	
	private static final String TAG = "SoundPoolSoundManager";

	private boolean enabled = true;
	private Context context;
	private SoundPool soundPool;
	private HashMap<String, Integer> soundPoolMap;
	private AudioManager mgr;
	private ArrayList<SoundStream> loopingStreamIds = new ArrayList<SoundStream>();

	public SoundPoolWrapper(Context context) {
		this.context = context;
		mgr = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.batterypoweredgames.lightracerelite.SoundManager#init()
	 */
	public void init(int streams) {
		if (enabled && mgr.getRingerMode() == AudioManager.RINGER_MODE_NORMAL) {
			Log.d(TAG, "Initializing new SoundPool");
			release();
			soundPool = new SoundPool(streams, AudioManager.STREAM_MUSIC, 0);
			soundPoolMap = new HashMap<String, Integer>();
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.batterypoweredgames.lightracerelite.SoundManager#release()
	 */
	public void release() {
		if (soundPool != null) {
			Log.d(TAG, "Closing SoundPool");
			soundPool.release();
			soundPool = null;
			Log.d(TAG, "SoundPool closed");
			return;
		}
	}

	public void setStreamRate(int streamId, float rate) {
		if (streamId != -1 && rate > .5 && rate < 1.5) {
			soundPool.setRate(streamId, rate);
		}
	}
	
	/**
	 * Sets a relative volume for the stream
	 * @param streamId
	 * @param volume a volume between 0 and 1, 0 being silent and 1 being 100%
	 */
	public void setStreamVolume(int streamId, float volume) {
		if (streamId != -1) {
			int streamVolume = mgr.getStreamVolume(AudioManager.STREAM_MUSIC);
			float vol = streamVolume * volume;
			soundPool.setVolume(streamId, vol, vol);
		}
	}
	
	public void loadSound(String assetName) {
		if (soundPool != null) {
			AssetFileDescriptor afd;
			try {
				afd = context.getAssets().openFd(assetName);
				soundPoolMap.put(assetName, soundPool.load(afd, 1));
			} catch (IOException e) {
				Log.e(TAG, "Error loading " + assetName + "(" + e.getMessage() + ")", e);
			}
		}
	}
	
	public int playSound(String assetName, float leftVol, float rightVol, int loops, float rate) {
		int streamId = -1;
		if (soundPool != null) {
			//Log.d(TAG, "Playing Sound " + sound);
			int streamVol = mgr.getStreamVolume(AudioManager.STREAM_MUSIC);
			float vol = streamVol / 15f;
			// made it exponential
			//Log.d(TAG, "Playing sound with stream volume=" + streamVolume);
			Integer soundId = soundPoolMap.get(assetName);
			if (soundId != null) {
				streamId = soundPool.play(soundPoolMap.get(assetName), vol * leftVol, vol * rightVol, 1, loops, rate);
				if (loops == -1) {
					loopingStreamIds.add(new SoundStream(assetName, streamId));
				}
			}
		}
		return streamId;
	}
	
	public void stopSound(int streamId) {
		if (soundPool != null) {
			//soundPool.stop(streamId);
			soundPool.setLoop(streamId, 0);
			soundPool.setVolume(streamId, 0, 0);
			for (int i = 0; i < loopingStreamIds.size(); i++) {
				SoundStream sStream = loopingStreamIds.get(i);
				if (sStream.streamId == streamId) {
					loopingStreamIds.remove(i);
					// back up one since we removed one
					i--;
				}
			}
		}
	}
	
	public void stopSound(String assetName) {
		// find all streams with this assetName and stop em
		if (soundPool != null) {
			for (int i = 0; i < loopingStreamIds.size(); i++) {
				SoundStream sStream = loopingStreamIds.get(i);
				if (sStream.assetName.equals(assetName)) {
					//soundPool.stop(sStream.streamId);
					soundPool.setLoop(sStream.streamId, 0);
					soundPool.setVolume(sStream.streamId, 0, 0);
					loopingStreamIds.remove(i);
					// back up one since we removed one
					i--;
				}
			}
		}
	}

	public void stopAllSounds() {
		if (soundPool != null) {
			for (SoundStream sStream : loopingStreamIds) {
				//soundPool.stop(sStream.streamId);
				soundPool.setLoop(sStream.streamId, 0);
				soundPool.setVolume(sStream.streamId, 0, 0);
			}
			loopingStreamIds.clear();
		}
	}
	
	public void unloadSound(String assetName) {
		if (soundPool != null) {
			Integer soundId = soundPoolMap.get(assetName);
			if (soundId != null) {
				soundPool.unload(soundId);
				soundPoolMap.remove(assetName);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.batterypoweredgames.lightracerelite.SoundManager#setEnabled(boolean)
	 */
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}
	
	private static class SoundStream {
		private String assetName;
		private int streamId;
		private SoundStream(String assetName, int streamId) {
			this.assetName = assetName;
			this.streamId = streamId;
		}
	}
}
