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
// Name        : PCMAudioManager.cpp
// Description : Low-level PCM Manager for audio playback using only a platform PCM Stream
// Usage       : Do not use directly.  Use AudioManager.  See PREFER_PLATFORM_AUDIO_MANAGEMENT
//             : in batterytech_globals.h to force using this over using platform services.
//============================================================================

#ifndef PCMSOUNDMANAGER_H_
#define PCMSOUNDMANAGER_H_

#include "../primitives.h"
#include "PCMSound.h"
#include "PCMStream.h"
#include "../util/ManagedArray.h"

namespace BatteryTech {

	class PCMAudioManager {
	public:
		PCMAudioManager();
		virtual ~PCMAudioManager();
		S32 loadSound(S16 *pcmData, U32 length, U32 sampleRate, U8 channels, const char *assetName);
		S32 loadSound(const char *assetName);
		S32 playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
		S32 playSound(S32 soundId, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
		void stopSound(S32 streamId);
		void stopSound(const char *assetName);
		void stopAllSounds();
		void unloadSound(const char *assetName);
		void unloadSound(S32 soundId);
		void init(U8 streams);
		S32 playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
		void stopStreamingSound(const char *assetName);
		void setLoops(S32 streamId, S16 loops);
		void setVolume(S32 streamId, F32 leftVol, F32 rightVol);
		void setRate(S32 streamId, F32 rate);
		void fillBuffer(void *pSoundBuffer, long bufferLen);
		void update();
	private:
		S32 getSoundId(const char *assetName);
        BOOL32 isSoundLoaded(const char *assetName);
		PCMSound* getSound(S32 soundId);
		void fillStreamingBuffer(PCMStream *pcmStream, U16 bufNum);
		U8 streamCount;
		ManagedArray<PCMSound> *pcmSounds;
		PCMStream *pcmStreams;
		BOOL32 isReady;
		S32 lastSoundId;
		S32 lastStreamId;
	};

}
#endif /* SOUNDMANAGER_H_ */
