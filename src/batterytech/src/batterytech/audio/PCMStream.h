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
// Name        : PCMStream.h
// Description : A representation of a currently playing PCM Stream used by PCMAudioManager.
//============================================================================

#ifndef PCMSTREAM_H_
#define PCMSTREAM_H_

#include "PCMSound.h"
#include "../batterytech_globals.h"

namespace BatteryTech {

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
		U32 resampleActiveRate;
		// resampleInt = resampleActiveRate / PCM_AUDIO_PLAYBACK_RATE
		U8 resampleInt;
		// resampleFrac = resampleActiveRate % PCM_AUDIO_PLAYBACK_RATE
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
		BOOL32 audioBufEmpty[PCM_AUDIO_STREAM_BUFFERS];
		U16 audioBufLimit[PCM_AUDIO_STREAM_BUFFERS];
		void* audioHandle;
		U16 maxFrameSize;
	};

}
#endif /* PCMSTREAM_H_ */
