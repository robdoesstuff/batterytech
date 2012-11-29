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
// Name        : PCMStream.cpp
// Description : A representation of a currently playing PCM Stream used by PCMAudioManager.
//============================================================================

#include "PCMStream.h"

namespace BatteryTech {

	PCMStream::PCMStream() {
		bufferPosition = 0;
		overallPosition = 0;
		isPlaying = FALSE;
		pcmSound = NULL;
		loopsRemaining = 0;
		playbackRate = 1.0f;
		streamId = 0;
		soundId = 0;
		volumeLeft = 1.0f;
		volumeRight = 1.0f;
		isStreaming = FALSE;
		tempLength = 0;
		length = 0;
		assetLength = 0;
		channels = 0;
		resampleActiveRate = 0;
		resampleInt = 0;
		resampleFrac = 0;
		resampleFracAccumulated = 0;
		assetName = NULL;
		filePosition = 0;
		dataBuf = NULL;
		sampleRate = 0;
		audioBuf = NULL;
		activeAudioBuf = 0;
		for (S32 i = 0; i < PCM_AUDIO_STREAM_BUFFERS; i++) {
			audioBufEmpty[i] = TRUE;
			audioBufLimit[i] = 0;
		}
		audioHandle = NULL;
		maxFrameSize = 0;
	}

	PCMStream::~PCMStream() {
		if (dataBuf) {
			delete dataBuf;
		}
		dataBuf = NULL;
		if (audioBuf) {
			delete audioBuf[0];
			delete audioBuf[1];
			delete [] audioBuf;
		}
		audioBuf = NULL;
	}

}
