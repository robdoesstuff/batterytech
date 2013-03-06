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
// Name        : LinuxAudioGW.h
// Description : Linux audio gateway
//============================================================================

#if defined(linux)

#ifndef LINUXAUDIOGW_H_
#define LINUXAUDIOGW_H_

#include "../../audio/AudioManager.h"
#include <alsa/asoundlib.h>
#include <pthread.h>

namespace BatteryTech {

class LinuxAudioGW {
public:
	LinuxAudioGW(AudioManager *audioManager);
	virtual ~LinuxAudioGW();
	void init();
	void release();
	void fillBuffer();
	bool paused;
	bool run;
private:
	char *audioBuf;
	AudioManager *audioManager;
	snd_pcm_t *playback_handle;
	pthread_t thread;
};


}

#endif /* LINUXAUDIOGW_H_ */
#endif
