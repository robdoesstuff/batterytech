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
// Name        : BlackberryAudioGW.cpp
// Description : Blackberry audio gateway
//============================================================================
#if defined(__QNXNTO__)

#include "BlackberryAudioGW.h"
#include "../../Logger.h"
#include <gulliver.h>
#include <pthread.h>

#include <string.h>
#include <stdio.h>

#define AUDIO_BUFFER_SIZE 1024

namespace BatteryTech {

static BlackberryAudioGW *gw;

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

BlackberryAudioGW::BlackberryAudioGW(AudioManager *audioManager) {
	this->audioManager = audioManager;
	// we should only have one instance of an audio gateway
	gw = this;
	audioBuf = new unsigned char[AUDIO_BUFFER_SIZE];
	paused = false;
	thread = 0;
}

BlackberryAudioGW::~BlackberryAudioGW() {
	gw = NULL;
	delete[] audioBuf;
	audioBuf = NULL;
}

void BlackberryAudioGW::fillBuffer() {
	audioManager->fillBuffer(audioBuf, AUDIO_BUFFER_SIZE);
	int written = snd_pcm_plugin_write(pcm_handle, audioBuf, AUDIO_BUFFER_SIZE);
	if (written < 0) {
		logmsg("Playback error");
		run = false;
	} else if (written < AUDIO_BUFFER_SIZE) {
		snd_pcm_channel_status_t status;
		memset(&status, 0, sizeof(status));
		status.channel = SND_PCM_CHANNEL_PLAYBACK;
		if (snd_pcm_plugin_status(pcm_handle, &status) < 0) {
			logmsg("Playback channel status error (underrun)");
			run = false;
		}
		if (status.status == SND_PCM_STATUS_READY || status.status == SND_PCM_STATUS_UNDERRUN) {
			if (snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK) < 0) {
				logmsg("Playback channel prepare error (underrun)");
				run = false;
			}
		}
	}
}

void BlackberryAudioGW::init() {
	logmsg("Initializing Blackberry Audio");
	int rtn;
	int card = -1;
	int dev = 0;
	int sample_rate;
	int sample_channels;
	int sample_bits;
	snd_pcm_channel_info_t pi;
	snd_mixer_group_t group;
	snd_pcm_channel_params_t pp;
	snd_pcm_channel_setup_t setup;
	if ((rtn = snd_pcm_open_preferred(&pcm_handle, &card, &dev, SND_PCM_OPEN_PLAYBACK)) < 0) {
		logmsg("Error opening PCM device");
		return;
	}
	sample_rate = ENDIAN_LE32(44100);
	sample_channels = ENDIAN_LE16(2);
	sample_bits = ENDIAN_LE16(16);
	//audioManager->fillBuffer();
	memset(&pi, 0, sizeof(pi));
	pi.channel = SND_PCM_CHANNEL_PLAYBACK;
	if ((rtn = snd_pcm_plugin_info(pcm_handle, &pi)) < 0) {
		logmsg("snd_pcm_plugin_info failed");
		return;
	}

	memset(&pp, 0, sizeof(pp));
	pp.mode = SND_PCM_MODE_BLOCK;
	pp.channel = SND_PCM_CHANNEL_PLAYBACK;
	pp.start_mode = SND_PCM_START_FULL;
	pp.stop_mode = SND_PCM_STOP_STOP;
	pp.buf.block.frag_size = pi.max_fragment_size;
	pp.buf.block.frags_max = 1; // What should this be?
	pp.buf.block.frags_min = 1;

	pp.format.interleave = 1;
	pp.format.rate = sample_rate;
	pp.format.voices = sample_channels;
	pp.format.format = SND_PCM_SFMT_S16_LE;

	strcpy(pp.sw_mixer_subchn_name, "Wave playback channel");
	if ((rtn = snd_pcm_plugin_params(pcm_handle, &pp)) < 0) {
		logmsg("snd_pcm_plugin_params failed");
		return;
	}
	if ((rtn = snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK)) < 0) {
		logmsg("snd_pcm_plugin_prepare failed");
		return;
	}

	memset(&setup, 0, sizeof(setup));
	memset(&group, 0, sizeof(group));
	setup.channel = SND_PCM_CHANNEL_PLAYBACK;
	setup.mixer_gid = &group.gid;
	if ((rtn = snd_pcm_plugin_setup(pcm_handle, &setup)) < 0) {
		logmsg("snd_pcm_plugin_setup failed");
		return;
	}
	if (group.gid.name[0] == 0) {
		logmsg("Mixer Pcm Group Not Set");
		return;
	}
	if ((rtn = snd_mixer_open(&mixer_handle, card, setup.mixer_device)) < 0) {
		logmsg("snd_mixer_open failed");
		return;
	}
	logmsg("BB PCM Channel Ready");
	run = true;
	int ret = pthread_create(&thread, NULL, gwThreadLoop, (void*) NULL);
	if (ret) {
		char buf[255];
		sprintf(buf, "Error creating audio thread - %d", ret);
		logmsg(buf);
	}
}

void BlackberryAudioGW::release() {
	logmsg("Releasing Blackberry Audio");
	run = false;
	void *status;
	pthread_join(thread, &status);
	thread = NULL;
	snd_mixer_close(mixer_handle);
	snd_pcm_close(pcm_handle);
}

} /* namespace BatteryTech */
#endif
