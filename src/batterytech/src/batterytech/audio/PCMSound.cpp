/*
 * PCMSound.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#include "PCMSound.h"
#include "../platform/platformgeneral.h"
#include <stdlib.h>

PCMSound::PCMSound(S16 *pcmData, U32 length, U32 rate, U8 channels, const char *assetName, S32 soundId) {
	this->pcmData = pcmData;
	this->length = length;
	this->rate = rate;
	this->channels = channels;
	this->assetName = assetName;
	this->soundId = soundId;
}

PCMSound::~PCMSound() {
	if (pcmData) {
		// we do not allocate the pcmData - the decoder does, but we are responsible for deallocating it when done.
		free(pcmData);
	}
}
