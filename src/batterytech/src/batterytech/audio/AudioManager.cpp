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
// Name        : AudioManager.cpp
// Description : Cross-platform Audio Management and Playback (Sound FX, Music)
// Usage       : Use this class to trigger sound effects and play music.  If PREFER_PLATFORM_AUDIO_MANAGEMENT
//             : in batterytech_globals.h is set to true, this will attempt to use platform facilities instead of
//             : raw PCM playback when available.  If it is set to false, this will always use raw PCM mixing and playback.
//             : When using raw PCM playback, streaming sounds will decode in real-time chunks of PCM_AUDIO_STREAM_BUFFER_SIZE.
//             : Add more buffers and decrease buffer size to decrease decoding lag in the main loop.
//============================================================================

#include "AudioManager.h"
#include "../Logger.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>
#include "PCMAudioManager.h"
#include "../batterytech_globals.h"
#include "../Context.h"
#include "../video/VideoManager.h"

namespace BatteryTech {

	AudioManager::AudioManager(Context *context) {
		this->context = context;
		usingPlatformAudioManagement = FALSE;
		pcmSoundMgr = NULL;
		pcmPlugins = new ManagedArray<PCMAudioPlugin>(MAX_PCM_PLUGINS);
		enabled = TRUE;
		if (context->appProperties->get("prefer_platform_audio_management")->getBoolValue() && _platform_implements_soundpool()) {
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
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			_platform_load_sound(mAssetName);
		} else {
			pcmSoundMgr->loadSound(assetName);
		}
	}

	S32 AudioManager::playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
		if (!enabled) {
			return -1;
		}
		if (usingPlatformAudioManagement) {
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			return _platform_play_sound(mAssetName, leftVol, rightVol, loops, rate);
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
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			_platform_stop_sound(mAssetName);
		} else {
			pcmSoundMgr->stopSound(assetName);
		}
	}

	void AudioManager::unloadSound(const char *assetName) {
		if (usingPlatformAudioManagement) {
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			_platform_unload_sound(mAssetName);
		} else {
			pcmSoundMgr->stopSound(assetName);
			pcmSoundMgr->unloadSound(assetName);
		}
	}

	void AudioManager::playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
		if (!enabled) {
			return;
		}
		if (usingPlatformAudioManagement) {
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			_platform_play_streaming_sound(mAssetName, loops, leftVol, rightVol, rate);
		} else {
			pcmSoundMgr->playStreamingSound(assetName, loops, leftVol, rightVol, rate);
		}
	}

	void AudioManager::stopStreamingSound(const char *assetName) {
		if (usingPlatformAudioManagement) {
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			_platform_stop_streaming_sound(mAssetName);
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

	void AudioManager::addPCMPlugin(PCMAudioPlugin *plugin) {
		pcmPlugins->add(plugin);
	}

	void AudioManager::removePCMPlugin(PCMAudioPlugin *plugin) {
		pcmPlugins->remove(plugin);
	}

	void AudioManager::fillBuffer(void *pSoundBuffer, long bufferLen) {
		if (pcmSoundMgr) {
			pcmSoundMgr->fillBuffer(pSoundBuffer, bufferLen);
			context->videoManager->addAudioBuffer(pSoundBuffer, bufferLen);
			for (S32 i = 0; i < pcmPlugins->getSize(); i++) {
				pcmPlugins->array[i]->addToBuffer(pSoundBuffer, bufferLen);
			}
		}
	}

	void AudioManager::update() {
		if (pcmSoundMgr) {
			pcmSoundMgr->update();
		}
	}

}
