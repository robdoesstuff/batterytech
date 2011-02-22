/*
 * PCMStream.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PCMSTREAM_H_
#define PCMSTREAM_H_

#include "PCMSound.h"
#include "../batterytech_globals.h"

class PCMStream {
public:
	PCMStream();
	virtual ~PCMStream();
	PCMSound *pcmSound;
	U32 bufferPosition;
	U32 overallPosition;
	S16 loopsRemaining;
	BOOL32 isPlaying;
	F32 playbackRate;
	S32 streamId;
	S32 soundId;
	F32 volumeLeft;
	F32 volumeRight;
	// update all resample fields when playbackRate changed
	// resampleActiveRate = sampleRate * playbackRate
	U16 resampleActiveRate;
	// resampleInt = resampleActiveRate / PLAYBACK_RATE
	U8 resampleInt;
	// resampleFrac = resampleActiveRate % PLAYBACK_RATE
	U32 resampleFrac;
	// accumulate the fraction until greater than active rate
	U32 resampleFracAccumulated;
	// for streaming
	BOOL32 isStreaming;
	// temp length is used to count how much data has been decoded until we hit EOF and can mark actual length
	U32 tempLength;
	U32 length;
	U32 assetLength;
	U8 channels;
	const char *assetName;
	U32 filePosition;
	unsigned char *dataBuf;
	U32 sampleRate;
	S16 **audioBuf;
	U16 activeAudioBuf;
	BOOL32 audioBufEmpty[STREAM_BUFFERS];
	U16 audioBufLimit[STREAM_BUFFERS];
	void* audioHandle;
	U16 maxFrameSize;
};

#endif /* PCMSTREAM_H_ */
