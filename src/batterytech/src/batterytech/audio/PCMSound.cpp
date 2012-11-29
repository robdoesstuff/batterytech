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
// Name        : PCMSound.cpp
// Description : A PCM Sound holding the actual PCM sound data.  Used by PCMAudioManager.
//============================================================================

#include "PCMSound.h"
#include "../platform/platformgeneral.h"
#include <stdlib.h>
#include "../util/strx.h"

namespace BatteryTech {

	PCMSound::PCMSound(S16 *pcmData, U32 length, U32 rate, U8 channels, const char *assetName, S32 soundId) {
		this->pcmData = pcmData;
		this->length = length;
		this->rate = rate;
		this->channels = channels;
		this->assetName = strDuplicate(assetName);
		this->soundId = soundId;
	}

	PCMSound::~PCMSound() {
		if (pcmData) {
			// we do not allocate the pcmData - the decoder does, but we are responsible for deallocating it when done.
			// the decoder uses malloc so we must free and not delete.
			free(pcmData);
		}
		delete [] assetName;
	}

}
