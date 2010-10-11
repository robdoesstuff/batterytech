/*
 * SoundManager.cpp
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#include "SoundManager.h"
#include "../Logger.h"
#include "../decoders/stb_vorbis.h"
#include "../platform/platformgeneral.h"

#define PLAYBACK_RATE 44100

SoundManager::SoundManager() {
	pcmSoundsLoaded = 0;
}

SoundManager::~SoundManager() {
}

S16 SoundManager::loadSound(const char *assetName) {
	int assetSize = 0;
	unsigned char *fileData = _platform_load_asset(assetName, &assetSize);
	S16 sndId = -1;
	if (fileData) {
		char buf[50];
		sprintf(buf, "Loaded %i bytes of ogg data", assetSize);
		logmsg(buf);
		signed short *decoded;
		int channels, len;
		unsigned int sampleRate = 0;
		len = stb_vorbis_decode_memory(fileData, assetSize, &channels, &sampleRate, &decoded);
		sprintf(buf, "OGG length=%i channels=%i rate=%i", len, channels, sampleRate);
		logmsg(buf);
		sndId = loadSound(decoded, len, sampleRate, channels);
	}
	_platform_free_asset(fileData);
	return sndId;
}

S16 SoundManager::loadSound(S16 *pcmData, U32 length, U32 rate, U8 channels) {
	PCMSound *pcmSound = new PCMSound(pcmData, length, rate, channels);
	S16 soundId = pcmSoundsLoaded;
	pcmSounds[pcmSoundsLoaded++] = pcmSound;
	//TODO - use array management
	return soundId;
}

void SoundManager::playSound(U16 soundId, S16 loops, F32 rate) {
	char buf[50];
	sprintf(buf, "Playing sound %i", soundId);
	logmsg(buf);
	// create a stream
	BOOL32 success = FALSE;
	for (int i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (!stream->isPlaying) {
			sprintf(buf, "Using Stream %i", i);
			logmsg(buf);
			// use this stream;
			PCMSound *sound = pcmSounds[soundId];
			stream->pcmSound = sound;
			stream->loops_remaining = loops;
			stream->playback_rate = rate;
			stream->position = 0;
			stream->isPlaying = TRUE;
			sprintf(buf, "stream->isPlaying=%i", stream->isPlaying);
			logmsg(buf);
			success = TRUE;
			break;
		}
	}
	if (!success) {
		logmsg("No streams left to play on");
	}
}

void SoundManager::unloadSound(U16 soundId) {
	// TODO - release the PCMSound which should release the PCMData
}

void SoundManager::fillBuffer(void *pSoundBuffer, long bufferLen) {
	//log("fillBuffer");
	signed short *pSample = (signed short*)pSoundBuffer;
	long	nbSample = bufferLen / sizeof(signed short);
	// clear the sample (TODO use memset!)
	int i, j, sampleIdx;
	for (i = 0; i < nbSample; i++) {
		pSample[i] = 0;
	}

	char buf[50];
	// for each stream, add the sample
	for (i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		//sprintf(buf, "stream %i isPlaying=%i", i, stream.isPlaying);
		//log(buf);
		if (stream->isPlaying) {
			PCMSound *pcmSound = stream->pcmSound;
			//sprintf(buf, "Streaming position %i of %i", stream->position, pcmSound->length);
			//log(buf);
			U8 ratio = PLAYBACK_RATE / pcmSound->rate;
			for (j=0; j<nbSample; j += ratio) {
				signed short sample = pcmSound->pcmData[stream->position++];
				// mix with headroom
				signed int mix;
				// resample (convert 44100, 22050, 11025)
				for (sampleIdx = 0; sampleIdx < ratio; sampleIdx++) {
					mix = pSample[j + sampleIdx] + sample;
					if (mix > 16384) {
						mix = 16384;
					} else if (mix < -16384) {
						mix = -16384;
					}
					pSample[j + sampleIdx] = (signed short) mix;
				}
				if (stream->position >= pcmSound->length) {
					// TODO - loops
					//sprintf(buf, "looping %i of %i", stream->position, pcmSound->length);
					//log(buf);
					stream->position = 0;
				}
			}
		}
	}
}

void SoundManager::init(U8 streams) {
	streamCount = streams;
	pcmSounds = new PCMSound*[MAX_SOUNDS];
	for (int i = 0; i < MAX_SOUNDS; i++) {
		pcmSounds[i] = NULL;
	}
	pcmStreams = new PCMStream[streams];
	_platform_init_sound(this);
}

void SoundManager::release() {
	for (int i = 0; i < MAX_SOUNDS; i++) {
		if (pcmSounds[i] != NULL) {
			pcmSounds[i]->release();
			delete pcmSounds[i];
		}
	}
	delete [] pcmSounds;
	delete [] pcmStreams;
}

