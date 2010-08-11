/*
 * SoundManager.h
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#define MAX_SOUNDS 200

#include "../primitives.h"
#include "PCMSound.h"
#include "PCMStream.h"

class SoundManager {
public:
	SoundManager();
	virtual ~SoundManager();
	S16 loadSound(S16 *pcmData, U32 length, U32 rate, U8 channels);
	S16 loadSound(const char *assetName);
	void playSound(U16 soundId, S16 loops, F32 rate);
	void unloadSound(U16 soundId);
	void init(U8 streams);
	void release();
	void fillBuffer(void *pSoundBuffer, long bufferLen);
private:
	U8 streamCount;
	U16 pcmSoundsLoaded;
	PCMSound **pcmSounds;
	PCMStream *pcmStreams;
};

#endif /* SOUNDMANAGER_H_ */
