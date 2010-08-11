/*
 * PCMSound.h
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#ifndef PCMSOUND_H_
#define PCMSOUND_H_

#include "../primitives.h"

class PCMSound {
public:
	PCMSound(S16 *pcmData, U32 length, U32 rate, U8 channels);
	virtual ~PCMSound();
	S16 *pcmData;
	U32 length;
	U32 rate;
	U8 channels;
	void release();
};

#endif /* PCMSOUND_H_ */
