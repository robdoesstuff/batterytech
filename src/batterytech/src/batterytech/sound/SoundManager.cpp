/*
 * SoundManager.cpp
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#include "SoundManager.h"
#include "../Logger.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>
#include "PCMSoundManager.h"

#define PREFER_PLATFORM_SOUNDPOOL TRUE

SoundManager::SoundManager() {
	usingNativeSoundPool = FALSE;
	pcmSoundMgr = NULL;
	enabled = TRUE;
}

void SoundManager::init(U8 streams) {
	if (PREFER_PLATFORM_SOUNDPOOL && _platform_implements_soundpool()) {
		usingNativeSoundPool = TRUE;
		_platform_init_soundpool(streams);
	} else {
		pcmSoundMgr = new PCMSoundManager();
		pcmSoundMgr->init(streams);
		_platform_init_sound(this);
	}
}

SoundManager::~SoundManager() {
	logmsg("Releasing SoundManager");
	if (usingNativeSoundPool) {
		_platform_release_soundpool();
	} else {
		_platform_stop_sound();
	}
	if (pcmSoundMgr) {
		delete pcmSoundMgr;
	}
	pcmSoundMgr = NULL;
}

void SoundManager::setEnabled(BOOL32 enabled) {
	this->enabled = enabled;
}

BOOL32 SoundManager::isEnabled() {
	return enabled;
}

void SoundManager::loadSound(const char *assetName) {
	if (usingNativeSoundPool) {
		_platform_load_sound(assetName);
	} else {
		pcmSoundMgr->loadSound(assetName);
	}
}

S32 SoundManager::playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	if (!enabled) {
		return -1;
	}
	if (usingNativeSoundPool) {
		return _platform_play_sound(assetName, leftVol, rightVol, loops, rate);
	} else {
		return pcmSoundMgr->playSound(assetName, loops, leftVol, rightVol, rate);
	}
}

void SoundManager::stopAllSounds() {
	if (!enabled) {
		return;
	}
	if (usingNativeSoundPool) {
		_platform_stop_all_sounds();
	} else {
		pcmSoundMgr->stopAllSounds();
	}
}

void SoundManager::stopSound(S32 streamId) {
	if (!enabled) {
		return;
	}
	if (usingNativeSoundPool) {
		_platform_stop_sound(streamId);
	} else {
		pcmSoundMgr->stopSound(streamId);
	}
}

void SoundManager::stopSound(const char *assetName) {
	if (!enabled) {
		return;
	}
	if (usingNativeSoundPool) {
		_platform_stop_sound(assetName);
	} else {
		pcmSoundMgr->stopSound(assetName);
	}
}

void SoundManager::unloadSound(const char *assetName) {
	if (usingNativeSoundPool) {
		_platform_unload_sound(assetName);
	} else {
		pcmSoundMgr->unloadSound(assetName);
	}
}

void SoundManager::playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	if (usingNativeSoundPool) {

	} else {
		pcmSoundMgr->playStreamingSound(assetName, loops, leftVol, rightVol, rate);
	}
}

void SoundManager::stopStreamingSound(const char *assetName) {
	if (usingNativeSoundPool) {

	} else {
		pcmSoundMgr->stopStreamingSound(assetName);
	}
}

void SoundManager::fillBuffer(void *pSoundBuffer, long bufferLen) {
	if (pcmSoundMgr) {
		pcmSoundMgr->fillBuffer(pSoundBuffer, bufferLen);
	}
}

void SoundManager::update() {
	if (pcmSoundMgr) {
		pcmSoundMgr->update();
	}
}
