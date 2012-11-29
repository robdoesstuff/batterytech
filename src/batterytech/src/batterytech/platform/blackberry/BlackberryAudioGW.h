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
// Name        : BlackberryAudioGW.h
// Description : Blackberry audio gateway
//============================================================================

#if defined(__QNXNTO__)

#ifndef BLACKBERRYAUDIOGW_H_
#define BLACKBERRYAUDIOGW_H_

#include "../../audio/AudioManager.h"
#include <sys/asoundlib.h>
#include <bps/audiomixer.h>
#include <pthread.h>

namespace BatteryTech {

class BlackberryAudioGW {
public:
	BlackberryAudioGW(AudioManager *audioManager);
	virtual ~BlackberryAudioGW();
	void init();
	void release();
	void fillBuffer();
	bool paused;
	bool run;
private:
	unsigned char *audioBuf;
	AudioManager *audioManager;
	snd_pcm_t *pcm_handle;
	snd_mixer_t *mixer_handle;
	pthread_t thread;
};


}

#endif /* BLACKBERRYAUDIOGW_H_ */
#endif
