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
// Name        : AudioManager.cpp
// Description : Cross-platform Audio Management and Playback (Sound FX, Music)
// Usage       : Use this class to trigger sound effects and play music.  If PREFER_PLATFORM_AUDIO_MANAGEMENT
//             : in batterytech_globals.h is set to true, this will attempt to use platform facilities instead of
//             : raw PCM playback when available.  If it is set to false, this will always use raw PCM mixing and playback.
//             : When using raw PCM playback, streaming sounds will decode in real-time chunks of PCM_AUDIO_STREAM_BUFFER_SIZE.
//             : Add more buffers and decrease buffer size to decrease decoding lag in the main loop.
//============================================================================

#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

#include <batterytech/primitives.h>

namespace BatteryTech {

	class Context;
	class PCMAudioManager;

	class AudioManager {
	public:
		AudioManager(Context *context);
		virtual ~AudioManager();
		/**
		 * Loads a sound.  Must be called before the sound can be played unless it is a streaming sound.
		 * Do not call for large, streaming sounds.
		 * Sound can be VERY memory consuming as it is decoded to PCM.  To calculate how much memory it is using,
		 * Multiply the sound length, rate and channels by 2 (for 16 bit).  60 seconds of 44.1khz mono = 5.3MB!
		 * For mobile, try to use 22.1khz mono sounds and trim empty sound off the ends to save space.
		 * Platform-specific implementations (when not using native PCM mixing) may not decode all sounds to memory.
		 */
		void loadSound(const char *assetName);
		/**
		 * Plays a sound that was loaded calling loadSound prior.
		 * loops specifies how many times to loop.  -1 is forever.
		 * leftVol and rightVol are 0 to 1.0 for max.
		 * rate is a % of playback rate.  1.0 is 100%, .5 = 50%, 1.5 = 150%, etc.
		 * Each sound is played on a stream.
		 * returns the streamId of the sound being played. (not to be confused with streamingSound)
		 */
		S32 playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
		/**
		 * Stops the sound's playback stream.
		 * Sorry for the confusion, but this is _not_ the method to call to stop streaming sound.
		 */
		void stopSound(S32 streamId);
		/**
		 * Stops the sound's stream using the assetName (recommended way to stop due to ease of code)
		 */
		void stopSound(const char *assetName);
		/**
		 * Stops all currently playing sounds.
		 */
		void stopAllSounds();
		/**
		 * Unloads the sound from memory.  See loadSound()
		 */
		void unloadSound(const char *assetName);
		/**
		 * Plays a streaming sound.  Streaming sounds should NOT be loaded ahead of time.
		 * The sound will be decoded in small chunks while playing.  See batterytech_globals for decoding performance settings.
		 */
		void playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
		/**
		 * Stops a streaming sound.  Do not use with non-streaming sounds.
		 */
		void stopStreamingSound(const char *assetName);
		/**
		 * Changes the number of loops for a sound's stream ID.
		 * Works on normal and streaming sound.
		 * May not work with platform-implemented streaming sound.
		 */
		void setLoops(S32 streamId, S16 loops);
		/**
		 * Changes the volume for a sound's stream.
		 * Works on normal and streaming sound.
		 * May not work with platform-implemented streaming sound.
		 */
		void setVolume(S32 streamId, F32 leftVol, F32 rightVol);
		/**
		 * Changes the playback rate for a sound's stream.
		 * Works on normal and streaming sound.
		 * May not work with platform-implemented streaming sound.
		 */
		void setRate(S32 streamId, F32 rate);
		/**
		 * Used by platform to route PCM data.  Do not use or modify or things may break.
		 */
		void fillBuffer(void *pSoundBuffer, long bufferLen);
		/**
		 * Enable or Disable audio here.
		 */
		void setEnabled(BOOL32 enabled);
		/**
		 * Is audio enabled?
		 */
		BOOL32 isEnabled();
		/**
		 * Needed to fill streaming sound buffers.
		 */
		void update();
	private:
		Context *context;
		S32 getSoundId(const char *assetName);
		BOOL32 usingPlatformAudioManagement;
		PCMAudioManager *pcmSoundMgr;
		BOOL32 enabled;
	};

}
#endif /* AUDIOMANAGER_H_ */
