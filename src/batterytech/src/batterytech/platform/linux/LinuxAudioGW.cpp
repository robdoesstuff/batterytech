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
// Name        : LinuxAudioGW.cpp
// Description : Linux audio gateway
//============================================================================
#if defined(linux)

#include "LinuxAudioGW.h"
#include "../../Logger.h"
#include <pthread.h>

#include <string.h>
#include <stdio.h>

// TODO - Add an auto-detect routine to get lowest latency HW configuration

#define AUDIO_BUFFER_SIZE 1024

namespace BatteryTech {

static LinuxAudioGW *gw;

static void* gwThreadLoop(void *param) {
	while (gw->run) {
		if (!gw->paused) {
			gw->fillBuffer();
		} else {
			// sleep 100ms while waiting to start up again
			usleep(100000);
		}
	}
	return NULL;
}

LinuxAudioGW::LinuxAudioGW(AudioManager *audioManager) {
	this->audioManager = audioManager;
	// we should only have one instance of an audio gateway
	gw = this;
	audioBuf = new char[AUDIO_BUFFER_SIZE];
	paused = false;
	thread = 0;
}

LinuxAudioGW::~LinuxAudioGW() {
	gw = NULL;
	delete[] audioBuf;
	audioBuf = NULL;
}

void LinuxAudioGW::fillBuffer() {
	audioManager->fillBuffer(audioBuf, AUDIO_BUFFER_SIZE);
	int written = snd_pcm_writei(playback_handle, audioBuf, AUDIO_BUFFER_SIZE/4);
	if (written < 0) {
		if (written == -EPIPE) {
			// prepare for playback again
			snd_pcm_prepare(playback_handle);
		} else {
			char buf[1024];
			sprintf(buf, "Playback error: %s\n", snd_strerror(written));
			logmsg(buf);
		}
		//run = false;
	} else if (written < AUDIO_BUFFER_SIZE/4) {
		logmsg("Underrun");
	} else {
		/*
		char buf[255];
		sprintf(buf, "Wrote %d frames", written);
		logmsg(buf);
		*/
	}
}

void LinuxAudioGW::init() {
	logmsg("Initializing Linux Audio");
	int err;
	snd_pcm_hw_params_t *hw_params;
	unsigned int playbackRate = 44100;
	int channels = 2;
	snd_pcm_uframes_t periodSize = 512;
	snd_pcm_uframes_t bufferSize = 2048;
	int dir;

	if ((err = snd_pcm_open (&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n", "default", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &playbackRate, &dir)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, channels)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n", snd_strerror (err));
	}


	if ((err = snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &periodSize, &dir)) < 0) {
		fprintf (stderr, "cannot set period size (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params_set_buffer_size_max(playback_handle, hw_params, &bufferSize)) < 0) {
		fprintf(stderr, "Error setting max buffersize (%s)\n", snd_strerror (err));
	}

	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n", snd_strerror (err));
	}

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (playback_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror (err));
	}

	fprintf(stdout, "Alsa PCM Channel Ready\n");
	snd_pcm_uframes_t period, buffer_size;
	snd_pcm_get_params(playback_handle, &buffer_size, &period);
	fprintf(stdout, "Buffer Size: %u, Period %u\n", buffer_size, period);
	run = true;
	int ret = pthread_create(&thread, NULL, gwThreadLoop, (void*) NULL);
	if (ret) {
		char buf[255];
		sprintf(buf, "Error creating audio thread - %d", ret);
		logmsg(buf);
	}
}

void LinuxAudioGW::release() {
	logmsg("Releasing Linux Audio");
	run = false;
	void *status;
	pthread_join(thread, &status);
	snd_pcm_close(playback_handle);
}

} /* namespace BatteryTech */
#endif
