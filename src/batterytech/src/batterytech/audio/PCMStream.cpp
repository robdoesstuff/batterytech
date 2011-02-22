/*
 * PCMStream.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#include "PCMStream.h"

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
	for (S32 i = 0; i < STREAM_BUFFERS; i++) {
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
