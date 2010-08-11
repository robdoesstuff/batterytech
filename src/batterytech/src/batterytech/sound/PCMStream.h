/*
 * PCMStream.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PCMSTREAM_H_
#define PCMSTREAM_H_

#include "PCMSound.h"

class PCMStream {
public:
	PCMStream();
	virtual ~PCMStream();
	PCMSound *pcmSound;
	U32 position;
	S16 loops_remaining;
	BOOL32 isPlaying;
	F32 playback_rate;
};

#endif /* PCMSTREAM_H_ */
