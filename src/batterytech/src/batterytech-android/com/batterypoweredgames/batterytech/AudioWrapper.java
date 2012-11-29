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
// Name        : AudioWrapper.java
// Description : Wraps Soundpool and Mediaplayer to provide platform-supported sound functionality
//============================================================================

package com.batterypoweredgames.batterytech;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.SoundPool;
import android.util.Log;

public class AudioWrapper {
	
	private static final String TAG = "AudioWrapper";

	private boolean enabled = true;
	private Context context;
	private SoundPool soundPool;
	private HashMap<String, Integer> soundPoolMap;
	private AudioManager mgr;
	private ArrayList<SoundStream> loopingStreamIds = new ArrayList<SoundStream>();
	private HashMap<String, MediaPlayer> streamingPlayers;

	public AudioWrapper(Context context) {
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
			streamingPlayers = new HashMap<String, MediaPlayer>();
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
		}
		if (streamingPlayers != null) {
			Log.d(TAG, "Stopping streaming players");
			for (String key : streamingPlayers.keySet()) {
				MediaPlayer mp = streamingPlayers.get(key);
				if (mp.isPlaying()) {
					mp.stop();
				}
				mp.release();
			}
			streamingPlayers.clear();
			streamingPlayers = null;
			Log.d(TAG, "Stopped streaming players");
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
			if (assetName.startsWith("file:")) {
				soundPoolMap.put(assetName, soundPool.load(assetName.substring(5), 1));
			} else {
				AssetFileDescriptor afd;
				try {
					afd = context.getAssets().openFd(assetName);
					soundPoolMap.put(assetName, soundPool.load(afd, 1));
				} catch (IOException e) {
					Log.e(TAG, "Error loading " + assetName + "(" + e.getMessage() + ")", e);
				}
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

	public void setLoops(int streamId, int loops) {
		if (soundPool != null && streamId != -1) {
			soundPool.setLoop(streamId, loops);
		}
	}
	
	public void setVolume(int streamId, float leftVol, float rightVol) {
		if (soundPool != null && streamId != -1) {
			int streamVol = mgr.getStreamVolume(AudioManager.STREAM_MUSIC);
			float vol = streamVol / 15f;
			soundPool.setVolume(streamId, vol * leftVol, vol * rightVol);	
		}
	}
	
	public void setRate(int streamId, float rate) {
		if (soundPool != null && streamId != -1) {
			soundPool.setRate(streamId, rate);
		}
	}

	public int playStreamingSound(String assetName, final float leftVol, final float rightVol, final int loops, float rate) {
		int streamId = -1;
		if (streamingPlayers != null) {
			if (streamingPlayers.get(assetName) != null) {
				return -1;
			}
			MediaPlayer mp = new MediaPlayer();
			if (loops == -1) {
				mp.setLooping(true);
			} else {
				mp.setLooping(false);
			}
			mp.setAudioStreamType(AudioManager.STREAM_MUSIC);
			mp.setVolume(leftVol, rightVol);
			streamingPlayers.put(assetName, mp);
			try {
				if (assetName.startsWith("file:")) {
					mp.setDataSource(assetName.substring(5));
				} else {
					AssetFileDescriptor afd = context.getAssets().openFd(assetName);
					mp.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
					afd.close();
				}
				mp.setOnPreparedListener(new OnPreparedListener() {
					   public void onPrepared(MediaPlayer mplayer) {
						   mplayer.setVolume(leftVol, rightVol);
						   if (loops == -1) {
							   mplayer.setLooping(true);
						   } else {
							   mplayer.setLooping(false);
						   }
						   mplayer.start();
					   }
					});
				mp.prepareAsync();
			} catch (IOException e) {
				Log.e(TAG, "Error starting media player");
			}
		}
		return streamId;
	}

	public void stopStreamingSound(String assetName) {
		if (streamingPlayers != null) {
			MediaPlayer mp = streamingPlayers.remove(assetName);
			if (mp == null) {
				return;
			}
			if (mp.isPlaying()) {
				mp.stop();
			}
			mp.release();
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
	
	public void onPause() {
		if (soundPool != null) {
			for (SoundStream sStream : loopingStreamIds) {
				//soundPool.stop(sStream.streamId);
				soundPool.pause(sStream.streamId);
			}
		}
		if (streamingPlayers != null) {
			for (String assetName : streamingPlayers.keySet()) {
				MediaPlayer mp = streamingPlayers.get(assetName);
				if (mp.isPlaying()) {
					mp.pause();
				}
			}
		}
	}
	
	public void onResume() {
		if (soundPool != null) {
			for (SoundStream sStream : loopingStreamIds) {
				//soundPool.stop(sStream.streamId);
				soundPool.resume(sStream.streamId);
			}
		}
		if (streamingPlayers != null) {
			for (String assetName : streamingPlayers.keySet()) {
				MediaPlayer mp = streamingPlayers.get(assetName);
				mp.start();
			}
		}
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
