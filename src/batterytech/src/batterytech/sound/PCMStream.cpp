/*
 * PCMStream.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: rgreen
 */

#include "PCMStream.h"

PCMStream::PCMStream() {
	position = 0;
	isPlaying = FALSE;
	pcmSound = NULL;
	loops_remaining = 0;
	playback_rate = 1.0f;
}

PCMStream::~PCMStream() {
}
