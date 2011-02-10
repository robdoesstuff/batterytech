/*
 * AudioManager.h
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

#include <batterytech/primitives.h>

class PCMAudioManager;

class AudioManager {
public:
	AudioManager();
	virtual ~AudioManager();
	void init(U8 streams);
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
