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
// Name        : VideoManager.cpp
// Description : Tie-ins for video playback - experimental
//============================================================================

#include "VideoManager.h"
#include "../Context.h"
#include "../platform/platformgeneral.h"
#include <stdio.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef WITH_FFMPEG
#ifdef _WIN32 
#define VM_USE_FFMPEG
#endif
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
#define VM_USE_FFMPEG
#endif /* MAC */
#endif

namespace BatteryTech {
	VideoManager::VideoManager(Context *context) {
		this->context = context;
	#ifdef VM_USE_FFMPEG
			ffmpegPlayer = NULL;
	#endif
	}

	VideoManager::~VideoManager() {
	#ifdef VM_USE_FFMPEG
			if (ffmpegPlayer) {
				delete ffmpegPlayer;
			}
			ffmpegPlayer = NULL;
	#endif
	}

	void VideoManager::load(const char *assetName) {
	#ifdef VM_USE_FFMPEG
			ffmpegPlayer = new btFFMpegPlayer(context, assetName);
	#else  
			char mAssetName[512];
			_platform_get_modified_asset_name(mAssetName, assetName);
			char buf[512];
			sprintf(buf, "loadVideo %s", mAssetName);
			_platform_hook(buf, NULL, 0);
	#endif
	}

	void VideoManager::play(BOOL32 loop) {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			ffmpegPlayer->play(loop);
		}
#else
		char buf[255];
		sprintf(buf, "playVideo ");
		if (loop) {
			strcat(buf, "loop");
		}
		_platform_hook(buf, NULL, 0);
#endif
	}

	void VideoManager::pause() {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			ffmpegPlayer->pause();
		}
#else
		_platform_hook("pauseVideo", NULL, 0);
#endif
	}

	F32 VideoManager::getPosition() {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			return ffmpegPlayer->getPosition();
		} else {
			return 0;
		}
#else
		char buf[255];
		buf[0] = '\0';
		_platform_hook("getVideoPosition", buf, 255);
		return atof(buf);
#endif
	}

	void VideoManager::stop() {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			ffmpegPlayer->stop();
		}
#else
		_platform_hook("stopVideo", NULL, 0);
#endif
	}

	void VideoManager::unload() {
#ifdef VM_USE_FFMPEG
		delete ffmpegPlayer;
		ffmpegPlayer = NULL;
#else
		_platform_hook("unloadVideo", NULL, 0);
#endif
	}

	void VideoManager::update(F32 delta) {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			ffmpegPlayer->update(delta);
		}
#endif
	}

	void VideoManager::render() {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			ffmpegPlayer->renderFrame();
		}
#endif
	}

	void VideoManager::addAudioBuffer(void *pSoundBuffer, long bufferLen) {
#ifdef VM_USE_FFMPEG
		if (ffmpegPlayer) {
			ffmpegPlayer->addAudioBuffer(pSoundBuffer, bufferLen);
		}
#endif
	}

}
