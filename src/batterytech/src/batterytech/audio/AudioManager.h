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
// Usage       : Use this class to trigger sound effects and play music.
//============================================================================

#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

#include <batterytech/primitives.h>

namespace BatteryTech {

	class Context;
	class PCMAudioManager;

	/**
	 * \ingroup Audio
	 * \brief Use this class to load and trigger sound effects and play music.
	 *
	 * This is the primary point of sound and streaming audio playback management for users. If "prefer_platform_audio_management"
	 * in batterytech_config.txt is set to "true", this will attempt to use platform facilities instead of
 	 * raw PCM playback when available.  If it is set to false, this will always use raw PCM mixing and playback.
 	 * When using raw PCM playback, streaming sounds will decode in real-time chunks of \ref PCM_AUDIO_STREAM_BUFFER_SIZE.
 	 * Add more buffers and decrease buffer size to decrease decoding lag in the main loop.
 	 *
 	 * Example:
 	 * context->audioManager->loadSound("sounds/mysound.ogg")
 	 * context->audioManager->playSound("sounds/mysound.ogg", 0, 1.0, 1.0, 1.0);
 	 * context->audioManager->unloadSound("sounds/mysound.ogg")
 	 *
	 */
	class AudioManager {
	public:
		AudioManager(Context *context);
		virtual ~AudioManager();

		/**
		 * \brief Loads a sound.
		 *
		 * Must be called before the sound can be played unless it is a streaming sound.
		 * Do not call for large, streaming sounds.
		 * Sound can be VERY memory consuming as it is decoded to PCM.  To calculate how much memory it is using,
		 * Multiply the sound length, rate and channels by 2 (for 16 bit).  60 seconds of 44.1khz mono = 5.3MB!
		 * For mobile, try to use 22.1khz mono sounds and trim empty sound off the ends to save space.
		 * Platform-specific implementations (when not using native PCM mixing) may not decode all sounds to memory.
		 *
		 * \param assetName The file asset name to load (must be OGG-Vorbis)
		 */
		void loadSound(const char *assetName);

		/**
		 * \brief Plays a sound that was loaded calling \ref loadSound prior.
		 *
		 * Each sound is played on a stream and needs to have already been loaded by \ref loadSound
		 *
		 * \param assetName The file asset name to play
		 * \param loops How many times to loop.  -1 is forever.
		 * \param leftVol The left channel volume ranging from 0.0 to 1.0
		 * \param rightVol The right channel volume ranging from 0.0 to 1.0
		 * \param rate The playback rate.  1.0 is 100%, .5 = 50%, 1.5 = 150%, etc.
		 *
		 * \returns the streamId of the sound being played. (not to be confused with streamingSound)
		 */
		S32 playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);

		/**
		 * \brief Stops a sound's playback stream.
		 *
		 * Sorry for the confusion, but this is _not_ the function to call to stop streaming sound.
		 *
		 * \param streamId The streamId returned by \ref playSound
		 */
		void stopSound(S32 streamId);

		/**
		 * \brief Stops the sound's stream using the assetName
		 *
		 * This is the recommended function to use to stop because you don't need to hold a streamId as a reference.  This is also not
		 * the function to call to stop streaming audio, but rather a sound started with \ref playSound
		 *
		 * \param assetName The asset filename that was started using \ref playSound
		 */
		void stopSound(const char *assetName);

		/**
		 * \brief Stops all currently playing sounds started with \ref playSound
		 */
		void stopAllSounds();

		/**
		 * \brief Unloads a sound that was previously loaded by \ref loadSound
		 *
		 * \param assetName The asset filename of the sound that was loaded by \ref loadSound
		 */
		void unloadSound(const char *assetName);

		/**
		 * \brief Plays a streaming sound.  Streaming sounds should NOT be loaded ahead of time.
		 *
		 * The sound will be decoded in small chunks while playing.  See \ref batterytech_globals.h for decoding performance settings.
		 *
		 * \param assetName The name of the file asset
		 * \param loops The number of loops to play
		 * \param leftVol The left channel volume ranging from 0.0 to 1.0
		 * \param rightVol The right channel volume ranging from 0.0 to 1.0
		 * \param rate The playback rate ranging from 0.5 to 1.5
		 */
		void playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);

		/**
		 * \brief Stops a streaming sound that was started with \ref playStreamingSound
		 *
		 * Do not use with non-streaming sounds.
		 *
		 * \param assetName The name of the file asset that was started with \ref playStreamingSound
		 */
		void stopStreamingSound(const char *assetName);

		/**
		 * \brief Changes the number of loops for a sound's stream ID.
		 *
		 * Works on normal and streaming sound.
		 * May not work with platform-implemented streaming sound.
		 *
		 * \param streamId The streamId returned by \ref playSound
		 * \param loops The number of loops to use
		 */
		void setLoops(S32 streamId, S16 loops);

		/**
		 * \brief Changes the volume for a sound's stream.
		 *
		 * Works on normal and streaming sound.
		 * May not work with platform-implemented streaming sound.
		 *
		 * \param streamId The streamId returned by \ref playSound
		 * \param leftVol The left channel volume ranging from 0.0 to 1.0
		 * \param rightVol The right channel volume ranging from 0.0 to 1.0
		 */
		void setVolume(S32 streamId, F32 leftVol, F32 rightVol);

		/**
		 * \brief Changes the playback rate for a sound's stream.
		 *
		 * Works on normal and streaming sound.
		 * May not work with platform-implemented streaming sound.
		 *
		 * \param streamId The streamId returned by \ref playSound
		 * \param rate The new playback rate ranging from 0.5 to 1.5
		 */
		void setRate(S32 streamId, F32 rate);

		/**
		 * \brief Used by platform to route PCM data.  Do not use or modify or things may break.
		 */
		void fillBuffer(void *pSoundBuffer, long bufferLen);
		/**
		 * \brief Enable or Disable audio here.
		 */
		void setEnabled(BOOL32 enabled);
		/**
		 * \brief Is audio enabled?
		 * \return \ref TRUE if enabled, \ref FALSE if not
		 */
		BOOL32 isEnabled();
		/**
		 * \brief Needed to fill streaming sound buffers and is normally called from the BatteryTech update system.
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
