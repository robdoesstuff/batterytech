/*
 * AudioManager.cpp
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#include "AudioManager.h"
#include "../Logger.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>
#include "PCMAudioManager.h"
#include "../batterytech_globals.h"

AudioManager::AudioManager() {
	usingPlatformAudioManagement = FALSE;
	pcmSoundMgr = NULL;
	enabled = TRUE;
	if (PREFER_PLATFORM_AUDIO_MANAGEMENT && _platform_implements_soundpool()) {
		usingPlatformAudioManagement = TRUE;
		_platform_init_audiomanagement(MAX_AUDIO_STREAMS);
	} else {
		pcmSoundMgr = new PCMAudioManager();
		pcmSoundMgr->init(MAX_AUDIO_STREAMS);
		_platform_init_sound(this);
	}
}

AudioManager::~AudioManager() {
	logmsg("Releasing AudioManager");
	if (usingPlatformAudioManagement) {
		_platform_release_audiomanagement();
	} else {
		_platform_stop_sound();
	}
	if (pcmSoundMgr) {
		delete pcmSoundMgr;
	}
	pcmSoundMgr = NULL;
}

void AudioManager::setEnabled(BOOL32 enabled) {
	this->enabled = enabled;
}

BOOL32 AudioManager::isEnabled() {
	return enabled;
}

void AudioManager::loadSound(const char *assetName) {
	if (usingPlatformAudioManagement) {
		_platform_load_sound(assetName);
	} else {
		pcmSoundMgr->loadSound(assetName);
	}
}

S32 AudioManager::playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	if (!enabled) {
		return -1;
	}
	if (usingPlatformAudioManagement) {
		return _platform_play_sound(assetName, leftVol, rightVol, loops, rate);
	} else {
		return pcmSoundMgr->playSound(assetName, loops, leftVol, rightVol, rate);
	}
}

void AudioManager::stopAllSounds() {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_stop_all_sounds();
	} else {
		pcmSoundMgr->stopAllSounds();
	}
}

void AudioManager::stopSound(S32 streamId) {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_stop_sound(streamId);
	} else {
		pcmSoundMgr->stopSound(streamId);
	}
}

void AudioManager::stopSound(const char *assetName) {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_stop_sound(assetName);
	} else {
		pcmSoundMgr->stopSound(assetName);
	}
}

void AudioManager::unloadSound(const char *assetName) {
	if (usingPlatformAudioManagement) {
		_platform_unload_sound(assetName);
	} else {
		pcmSoundMgr->unloadSound(assetName);
	}
}

void AudioManager::playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_play_streaming_sound(assetName, loops, leftVol, rightVol, rate);
	} else {
		pcmSoundMgr->playStreamingSound(assetName, loops, leftVol, rightVol, rate);
	}
}

void AudioManager::stopStreamingSound(const char *assetName) {
	if (usingPlatformAudioManagement) {
		_platform_stop_streaming_sound(assetName);
	} else {
		pcmSoundMgr->stopStreamingSound(assetName);
	}
}

void AudioManager::setLoops(S32 streamId, S16 loops) {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_sound_set_loops(streamId, loops);
	} else {
		pcmSoundMgr->setLoops(streamId, loops);
	}
}

void AudioManager::setVolume(S32 streamId, F32 leftVol, F32 rightVol) {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_sound_set_volume(streamId, leftVol, rightVol);
	} else {
		pcmSoundMgr->setVolume(streamId, leftVol, rightVol);
	}
}

void AudioManager::setRate(S32 streamId, F32 rate) {
	if (!enabled) {
		return;
	}
	if (usingPlatformAudioManagement) {
		_platform_sound_set_rate(streamId, rate);
	} else {
		pcmSoundMgr->setRate(streamId, rate);
	}
}

void AudioManager::fillBuffer(void *pSoundBuffer, long bufferLen) {
	if (pcmSoundMgr) {
		pcmSoundMgr->fillBuffer(pSoundBuffer, bufferLen);
	}
}

void AudioManager::update() {
	if (pcmSoundMgr) {
		pcmSoundMgr->update();
	}
}
