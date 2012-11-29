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
// Name        : PCMSound.h
// Description : A PCM Sound holding the actual PCM sound data.  Used by PCMAudioManager.
//============================================================================

#ifndef PCMSOUND_H_
#define PCMSOUND_H_

#include "../primitives.h"

namespace BatteryTech {

	class PCMSound {
	public:
		PCMSound(S16 *pcmData, U32 length, U32 rate, U8 channels, const char *assetName, S32 soundId);
		virtual ~PCMSound();
		S16 *pcmData;
		U32 length;
		U32 rate;
		U8 channels;
		S32 soundId;
		char *assetName;
	};

}
#endif /* PCMSOUND_H_ */
