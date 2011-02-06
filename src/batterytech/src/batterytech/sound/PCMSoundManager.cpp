/*
 * SoundManager.cpp
 *
 *  Created on: Aug 10, 2010
 *      Author: rgreen
 */

#include "PCMSoundManager.h"
#include "../Logger.h"
#include "../decoders/stb_vorbis.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>

#define PLAYBACK_RATE 44100

//FILE *testOutFile;

PCMSoundManager::PCMSoundManager() {
	pcmSoundsLoaded = 0;
	isReady = FALSE;
	lastStreamId = 0;
	streamCount = 0;
	pcmStreams = NULL;
	pcmSounds = NULL;
}

PCMSoundManager::~PCMSoundManager() {
	logmsg("Releasing PCMSoundManager");
	//fclose(testOutFile);
	for (int i = 0; i < MAX_SOUNDS; i++) {
		if (pcmSounds[i] != NULL) {
			pcmSounds[i]->release();
			delete pcmSounds[i];
		}
	}
	delete [] pcmSounds;
	delete [] pcmStreams;
	pcmSounds = NULL;
	pcmStreams = NULL;
	pcmSoundsLoaded = 0;
	isReady = FALSE;
	streamCount = 0;
}

S32 PCMSoundManager::loadSound(const char *assetName) {
	int assetSize = 0;
	unsigned char *fileData = _platform_load_asset(assetName, &assetSize);
	S32 sndId = -1;
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
		sndId = loadSound(decoded, len, sampleRate, channels, assetName);
	}
	_platform_free_asset(fileData);
	return sndId;
}

S32 PCMSoundManager::loadSound(S16 *pcmData, U32 length, U32 rate, U8 channels, const char *assetName) {
	PCMSound *pcmSound = new PCMSound(pcmData, length, rate, channels, assetName);
	S32 soundId = pcmSoundsLoaded;
	pcmSounds[pcmSoundsLoaded++] = pcmSound;
	//TODO - use array management
	return soundId;
}

S32 PCMSoundManager::playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	logmsg("Playsound");
	logmsg(assetName);
	return playSound(getSoundId(assetName), loops, leftVol, rightVol, rate);
}

S32 PCMSoundManager::getSoundId(const char *assetName) {
	S32 i;
	for (i = 0; i < pcmSoundsLoaded; i++) {
		if (strcmp(pcmSounds[i]->assetName, assetName) == 0) {
			return i;
		}
	}
	char buf[50];
	sprintf(buf, "Sound %s not loaded", assetName);
	logmsg(buf);
	return -1;
}

S32 PCMSoundManager::playSound(S32 soundId, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
	if (soundId == -1) {
		return -1;
	}
	if (leftVol < 0) {
		leftVol = 0;
	} else if (leftVol > 1) {
		leftVol = 1;
	}
	if (rightVol < 0) {
		rightVol = 0;
	} else if (rightVol > 1) {
		rightVol = 1;
	}
	S32 streamId = -1;
	//char buf[50];
	//sprintf(buf, "Playing sound %i", soundId);
	//logmsg(buf);
	// create a stream
	BOOL32 success = FALSE;
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (!stream->isPlaying) {
			//sprintf(buf, "Using Stream %i", i);
			//logmsg(buf);
			// use this stream;
			PCMSound *sound = pcmSounds[soundId];
			stream->pcmSound = sound;
			stream->loops_remaining = loops;
			stream->playback_rate = rate;
			stream->position = 0;
			stream->isPlaying = TRUE;
			streamId = ++lastStreamId;
			stream->streamId = streamId;
			stream->soundId = soundId;
			stream->volume_left = leftVol;
			stream->volume_right = rightVol;
			//sprintf(buf, "stream->isPlaying=%i", stream->isPlaying);
			//logmsg(buf);
			success = TRUE;
			break;
		}
	}
	if (!success) {
		logmsg("No streams left to play on");
	}
	return streamId;
}

void PCMSoundManager::stopAllSounds() {
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying) {
			stream->isPlaying = FALSE;
			stream->position = 0;
			stream->loops_remaining = 0;
		}
	}
}

void PCMSoundManager::stopSound(S32 streamId) {
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying && stream->streamId == streamId) {
			stream->isPlaying = FALSE;
			stream->position = 0;
			stream->loops_remaining = 0;
		}
	}
}

void PCMSoundManager::stopSound(const char *assetName) {
	S32 soundId = getSoundId(assetName);
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying && stream->soundId == soundId) {
			stream->isPlaying = FALSE;
			stream->position = 0;
			stream->loops_remaining = 0;
		}
	}
}

void PCMSoundManager::unloadSound(const char *assetName) {
	// TODO - implement
}

void PCMSoundManager::unloadSound(S32 soundId) {
	// TODO - release the PCMSound which should release the PCMData
}

void PCMSoundManager::fillBuffer(void *pSoundBuffer, long bufferLen) {
	//logmsg("fillBuffer");
	S32 channels = 2;
	S16 *pSample = (S16*)pSoundBuffer;
	long nbSample = bufferLen / sizeof(S16);
	int i, j, sampleIdx, channelIdx;
	memset(pSample, 0, bufferLen);
	if (!isReady) {
		return;
	}
	//char buf[60];
	// for each stream, add the sample
	for (i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		// 2 channels max
		F32 volumes[2];
		for (j = 0; j < channels; j++) {
			// we only support two channels, this is just a little optimization to remove branching in critical mixing section.
			if (j == 0) {
				volumes[j] = stream->volume_left;
			} else {
				volumes[j] = stream->volume_right;
			}
		}
		if (stream->isPlaying) {
			//sprintf(buf, "stream %i isPlaying %s", i, stream->pcmSound->assetName);
			//logmsg(buf);
			PCMSound *pcmSound = stream->pcmSound;
			//sprintf(buf, "Streaming position %i of %i", stream->position, pcmSound->length);
			//logmsg(buf);
			U8 ratio = PLAYBACK_RATE / pcmSound->rate;
			// process stream samples
			for (j=0; j<nbSample; j += ratio * channels) {
				// THIS ASSUMES MONO SAMPLES OF LOWER RATE THAN THE OUTPUT STREAM
				U32 pos = stream->position++;
				S16 sample = pcmSound->pcmData[pos];
				// mix with 32 bit headroom
				S32 mix;
				// for each channel (interleaved)
				for (channelIdx = 0; channelIdx < channels; channelIdx++) {
					// TODO - why do unrelated things crash on delete after playing 11khz samples??
					// resample (convert 44100, 22050, 11025)
					for (sampleIdx = 0; sampleIdx < ratio; sampleIdx++) {
						mix = pSample[j + sampleIdx + channelIdx * ratio] + (sample * volumes[channelIdx]);
						//mix = sample;
						// clip to signed short limits
						if (mix > 32767) {
							mix = 32767;
						} else if (mix < -32767) {
							mix = -32767;
						}
						pSample[j + sampleIdx + channelIdx * ratio] = (S16) mix;
					}
				}
				if (stream->position >= pcmSound->length) {
					//sprintf(buf, "looping %i of %i", stream->position, pcmSound->length);
					//log(buf);
					stream->position = 0;
					if (stream->loops_remaining == 0) {
						// done
						stream->isPlaying = FALSE;
						break;
					} else {
						if (stream->loops_remaining != -1) {
							stream->loops_remaining--;
						}
					}
				}
			}
		}
	}
	//fwrite(pSample, sizeof(S16), nbSample, testOutFile);
}

void PCMSoundManager::init(U8 streams) {
	streamCount = streams;
	pcmSounds = new PCMSound*[MAX_SOUNDS];
	for (int i = 0; i < MAX_SOUNDS; i++) {
		pcmSounds[i] = NULL;
	}
	pcmStreams = new PCMStream[streams];
	isReady = TRUE;
	//testOutFile = fopen("output.wav", "wb");
}

