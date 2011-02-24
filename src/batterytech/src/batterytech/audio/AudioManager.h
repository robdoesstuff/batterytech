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

class PCMAudioManager;

class AudioManager {
public:
	AudioManager();
	virtual ~AudioManager();
	void loadSound(const char *assetName);
	S32 playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
	void stopSound(S32 streamId);
	void stopSound(const char *assetName);
	void stopAllSounds();
	void unloadSound(const char *assetName);
	void playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
	void stopStreamingSound(const char *assetName);
	void setLoops(S32 streamId, S16 loops);
	void setVolume(S32 streamId, F32 leftVol, F32 rightVol);
	void setRate(S32 streamId, F32 rate);
	void fillBuffer(void *pSoundBuffer, long bufferLen);
	void setEnabled(BOOL32 enabled);
	BOOL32 isEnabled();
	void update();
private:
	S32 getSoundId(const char *assetName);
	BOOL32 usingPlatformAudioManagement;
	PCMAudioManager *pcmSoundMgr;
	BOOL32 enabled;
};

#endif /* AUDIOMANAGER_H_ */
