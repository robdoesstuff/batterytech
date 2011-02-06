/*
 * SoundManager.h
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <batterytech/primitives.h>

class PCMSoundManager;

class SoundManager {
public:
	SoundManager();
	virtual ~SoundManager();
	void init(U8 streams);
	void loadSound(const char *assetName);
	S32 playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate);
	void stopSound(S32 streamId);
	void stopSound(const char *assetName);
	void stopAllSounds();
	void unloadSound(const char *assetName);
	void fillBuffer(void *pSoundBuffer, long bufferLen);
	void setEnabled(BOOL32 enabled);
	BOOL32 isEnabled();
private:
	S32 getSoundId(const char *assetName);
	BOOL32 usingNativeSoundPool;
	PCMSoundManager *pcmSoundMgr;
	BOOL32 enabled;
};

#endif /* SOUNDMANAGER_H_ */
