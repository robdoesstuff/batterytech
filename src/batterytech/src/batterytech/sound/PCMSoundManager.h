/*
 * SoundManager.h
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#ifndef PCMSOUNDMANAGER_H_
#define PCMSOUNDMANAGER_H_

#define MAX_SOUNDS 200

#include "../primitives.h"
#include "PCMSound.h"
#include "PCMStream.h"

class PCMSoundManager {
public:
	PCMSoundManager();
	virtual ~PCMSoundManager();
	S32 loadSound(S16 *pcmData, U32 length, U32 rate, U8 channels, const char *assetName);
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
	void fillBuffer(void *pSoundBuffer, long bufferLen);
	void update();
private:
	S32 getSoundId(const char *assetName);
	void fillStreamingBuffer(PCMStream *pcmStream, U16 bufNum);
	U8 streamCount;
	S32 pcmSoundsLoaded;
	PCMSound **pcmSounds;
	PCMStream *pcmStreams;
	BOOL32 isReady;
	S32 lastStreamId;
};

#endif /* SOUNDMANAGER_H_ */
