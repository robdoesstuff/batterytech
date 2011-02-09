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

struct stb_vorbis;

#define PLAYBACK_RATE 44100
#define PLAYBACK_CHANNELS 2

FILE *testOutFile;

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
	fclose(testOutFile);
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
			stream->isStreaming = FALSE;
			PCMSound *sound = pcmSounds[soundId];
			stream->pcmSound = sound;
			stream->loopsRemaining = loops;
			stream->playbackRate = rate;
			stream->bufferPosition = 0;
			stream->overallPosition = 0;
			stream->isPlaying = TRUE;
			streamId = ++lastStreamId;
			stream->streamId = streamId;
			stream->soundId = soundId;
			stream->volumeLeft = leftVol;
			stream->volumeRight = rightVol;
			stream->length = sound->length;
			stream->tempLength = 0;
			stream->channels = sound->channels;
			stream->assetName = sound->assetName;
			stream->sampleRate = sound->rate;
			stream->resampleActiveRate = stream->sampleRate * stream->playbackRate;
			stream->resampleInt = stream->resampleActiveRate / PLAYBACK_RATE;
			stream->resampleFrac = stream->resampleActiveRate % PLAYBACK_RATE;
			stream->resampleFracAccumulated = 0;
			stream->filePosition = 0;
			stream->dataBuf = 0;
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
		if (stream->isPlaying && !stream->isStreaming) {
			stream->isPlaying = FALSE;
			stream->bufferPosition = 0;
			stream->overallPosition = 0;
			stream->loopsRemaining = 0;
		}
	}
}

void PCMSoundManager::stopSound(S32 streamId) {
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying && stream->streamId == streamId) {
			stream->isPlaying = FALSE;
			stream->bufferPosition = 0;
			stream->overallPosition = 0;
			stream->loopsRemaining = 0;
		}
	}
}

void PCMSoundManager::stopSound(const char *assetName) {
	S32 soundId = getSoundId(assetName);
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying && stream->soundId == soundId) {
			stream->isPlaying = FALSE;
			stream->bufferPosition = 0;
			stream->overallPosition = 0;
			stream->loopsRemaining = 0;
		}
	}
}

S32 PCMSoundManager::playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
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
			stream->isStreaming = TRUE;
			stream->pcmSound = NULL;
			stream->loopsRemaining = loops;
			stream->bufferPosition = 0;
			stream->overallPosition = 0;
			streamId = ++lastStreamId;
			stream->streamId = streamId;
			stream->volumeLeft = leftVol;
			stream->volumeRight = rightVol;
			stream->assetName = assetName;
			stream->channels = 0;
			stream->playbackRate = rate;
			stream->filePosition = 0;
			stream->assetLength = _platform_get_asset_length(assetName);
			if (stream->assetLength > 0) {
				// get vorbis handle and read info
				// fill both audio buffers
				if (!stream->audioBuf) {
					stream->audioBuf = new S16*[2];
					stream->audioBuf[0] = new S16[50000];
					stream->audioBuf[1] = new S16[50000];
				}
				unsigned char buf[16384];
				BOOL32 eof = FALSE;
				S32 bytesRead = _platform_read_asset_chunk(assetName, 0, buf, 16384, &eof);
				S32 bytesConsumed;
				S32 error;
				// start vorbis pushdata
				stb_vorbis *vorb = stb_vorbis_open_pushdata(buf, bytesRead, &bytesConsumed, &error, NULL);
				stb_vorbis_info info = stb_vorbis_get_info(vorb);
				stream->channels = info.channels;
				stream->sampleRate = info.sample_rate;
				stream->maxFrameSize = info.max_frame_size;
				stream->length = 0; // unknown until we hit eof
				stream->tempLength = 0;
				stream->filePosition = bytesConsumed;
				stream->audioHandle = vorb;
				stream->resampleActiveRate = stream->sampleRate * stream->playbackRate;
				stream->resampleInt = stream->resampleActiveRate / PLAYBACK_RATE;
				stream->resampleFrac = stream->resampleActiveRate % PLAYBACK_RATE;
				stream->resampleFracAccumulated = 0;
				fillStreamingBuffer(stream, 0);
				fillStreamingBuffer(stream, 1);
				stream->activeAudioBuf = 0;
				// set isPlaying to true last
				stream->isPlaying = TRUE;
			}
			success = TRUE;
			break;
		}
	}
	if (!success) {
		logmsg("No streams left to play on");
	}
	return streamId;

}

void PCMSoundManager::stopStreamingSound(const char *assetName) {

}

void PCMSoundManager::fillStreamingBuffer(PCMStream *stream, U16 bufNum) {
	BOOL32 done = FALSE;
	U32 audioBufPos = 0;
	char cBuf[50];
	sprintf(cBuf, "Filling bufNum %d", bufNum);
	logmsg(cBuf);
	U32 startingFilePos = stream->filePosition;
	while (!done) {
		// read data
		unsigned char buf[16384];
		sprintf(cBuf, "Reading at %d", stream->filePosition);
		logmsg(cBuf);
		BOOL32 eof = FALSE;
		S32 bytesRead = _platform_read_asset_chunk(stream->assetName, stream->filePosition, buf, 16384, &eof);
		S32 eofByte = 0;
		if (eof) {
			eofByte = bytesRead;
			if (bytesRead < 16384) {
				eof = FALSE;
				sprintf(cBuf, "Reading at 0");
				bytesRead += _platform_read_asset_chunk(stream->assetName, 0, buf + bytesRead, 16384 - bytesRead, &eof);
			}
		}
		S32 bytesConsumed = 0;
		// fill up audio buffer
		F32 **audioBuf;
		S32 samplesDecoded = 0;
		S32 channelsUsed = 0;
		BOOL32 hasMoreData = TRUE;
		S16 *streamAudioBuf = stream->audioBuf[bufNum];
		while (hasMoreData && audioBufPos + stream->maxFrameSize < 50000) {
			S32 dBytesConsumed = stb_vorbis_decode_frame_pushdata((stb_vorbis*) stream->audioHandle, buf + bytesConsumed, bytesRead - bytesConsumed, &channelsUsed, &audioBuf, &samplesDecoded);
			if (dBytesConsumed < 0) {
				logmsg("vorbis decoder error");
			} else if (dBytesConsumed == 0 && samplesDecoded == 0) {
				hasMoreData = FALSE;
			} else if (samplesDecoded) {
				bytesConsumed += dBytesConsumed;
				// convert to interleaved short
				if (channelsUsed == 1) {
					// mono
					for (S32 i = 0; i < samplesDecoded; i++) {
						// quick convert to 32 bit sample, cap
						S32 sample = ((S32) ((audioBuf[0][i]) * (1 << (15))));
						if ((unsigned int) (sample + 32768) > 65535) {
							sample = sample < 0 ? -32768 : 32767;
						}
						streamAudioBuf[audioBufPos + i] = sample;
					}
				} else if (channelsUsed == 2) {
					// stereo interleave
					for (S32 i = 0; i < samplesDecoded; i++) {
						S32 sample = ((S32) ((audioBuf[0][i]) * (1 << (15))));
						if ((unsigned int) (sample + 32768) > 65535) {
							sample = sample < 0 ? -32768 : 32767;
						}
						streamAudioBuf[audioBufPos + (i*2)] = sample;
						sample = ((S32) ((audioBuf[1][i]) * (1 << (15))));
						if ((unsigned int) (sample + 32768) > 65535) {
							sample = sample < 0 ? -32768 : 32767;
						}
						streamAudioBuf[audioBufPos + (i*2)+1] = sample;
					}
				}
				if (!stream->length) {
					stream->tempLength += samplesDecoded;
				}
				if (eofByte && bytesConsumed >= eofByte) {
					eofByte = 0;
					if (!stream->length) {
						stream->length = stream->tempLength + audioBufPos;
						stream->tempLength = 0;
					}
				}
				audioBufPos += samplesDecoded * channelsUsed;
			} else {
				bytesConsumed += dBytesConsumed;
			}
		}
		stream->audioBufLimit[bufNum] = audioBufPos;
		stream->filePosition += bytesConsumed;
		stream->filePosition %= stream->assetLength;
		stream->audioBufEmpty[bufNum] = FALSE;
		if (hasMoreData) {
			// buffer is full
			done = TRUE;
			if (stream->filePosition < 0) {
				stream->filePosition = startingFilePos + bytesConsumed;
			}
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
	U8 playbackChannels = PLAYBACK_CHANNELS;
	U16 playbackRate = PLAYBACK_RATE;
	S16 *pSample = (S16*)pSoundBuffer;
	long nbSample = bufferLen / sizeof(S16);
	int i, j, channelIdx;
	memset(pSample, 0, bufferLen);
	if (!isReady) {
		return;
	}
	//char buf[60];
	// for each stream, add the sample
	for (i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying) {
			// 2 channels max
			F32 volumes[2];
			for (j = 0; j < playbackChannels; j++) {
				// we only support two channels, this is just a little optimization to remove branching in critical mixing section.
				if (j == 0) {
					volumes[j] = stream->volumeLeft;
				} else {
					volumes[j] = stream->volumeRight;
				}
			}
			PCMSound *pcmSound = stream->pcmSound;
			// process stream samples
			S16 sample[2]; // 2 channels input max
			U32 bufferEnd;
			U32 streamEnd;
			BOOL32 isStreaming = stream->isStreaming;
			U8 activeAudioBuf = stream->activeAudioBuf;
			U8 sourceChannels = stream->channels;
			S16 *sampleSource;
			if (isStreaming) {
				sampleSource = stream->audioBuf[activeAudioBuf];
			} else {
				sampleSource = pcmSound->pcmData;
			}
			if (isStreaming) {
				bufferEnd = stream->audioBufLimit[activeAudioBuf];
				streamEnd = stream->length;
				if (stream->audioBufEmpty[0] && stream->audioBufEmpty[1]) {
					// underrun, wait for fill
					continue;
				}
			} else {
				streamEnd = bufferEnd = pcmSound->length;
			}
			char cBuf[50];
			sprintf(cBuf, "buf %d at %d of %d", activeAudioBuf, stream->bufferPosition, bufferEnd);
			logmsg(cBuf);
			for (j=0; j<nbSample; j += playbackChannels) {
				sample[0] = sampleSource[stream->bufferPosition];
				// mix with 32 bit headroom
				S32 mix;
				// for each output channel (interleaved)
				for (channelIdx = 0; channelIdx < playbackChannels; channelIdx++) {
					mix = pSample[j + channelIdx] + (sample[0] * volumes[channelIdx]);
					if ((U32) (mix + 32768) > 65535) {
						mix = mix < 0 ? -32767 : 32767;
					}
					pSample[j + channelIdx] = (S16) mix;
				}
				// update source position
				stream->bufferPosition += stream->resampleInt;
				stream->overallPosition += stream->resampleInt;
				stream->resampleFracAccumulated += stream->resampleFrac;
				if (stream->resampleFracAccumulated >= playbackRate) {
					stream->resampleFracAccumulated -= playbackRate;
					stream->bufferPosition++;
					stream->overallPosition++;
				}
				if (stream->bufferPosition >= bufferEnd) {
					stream->bufferPosition = 0;
					if (isStreaming) {
						stream->audioBufEmpty[activeAudioBuf] = TRUE;
						if (!stream->audioBufEmpty[!activeAudioBuf]) {
							stream->activeAudioBuf = activeAudioBuf = !activeAudioBuf;
							sampleSource = stream->audioBuf[activeAudioBuf];
							bufferEnd = stream->audioBufLimit[activeAudioBuf];
							logmsg("Switching buffers");
							continue;
						} else {
							// logmsg("Out of streaming data");
							// out of buffered data for stream
							continue;
						}
					}
				}
				if (streamEnd && stream->overallPosition >= streamEnd) {
					stream->overallPosition = 0;
					if (stream->loopsRemaining == 0) {
						// done
						stream->isPlaying = FALSE;
						break;
					} else {
						if (stream->loopsRemaining != -1) {
							stream->loopsRemaining--;
						}
					}
				}
			}
		}
	}
	fwrite(pSample, sizeof(S16), nbSample, testOutFile);
}

void PCMSoundManager::init(U8 streams) {
	streamCount = streams;
	pcmSounds = new PCMSound*[MAX_SOUNDS];
	for (int i = 0; i < MAX_SOUNDS; i++) {
		pcmSounds[i] = NULL;
	}
	pcmStreams = new PCMStream[streams];
	isReady = TRUE;
	testOutFile = fopen("output.wav", "wb");
}

void PCMSoundManager::update() {
	// check streams
	for (S32 i = 0; i < streamCount; i++) {
		PCMStream *stream = &pcmStreams[i];
		if (stream->isPlaying && stream->isStreaming) {
			//fill empty buffers (will normally only be 1 to fill unless app was interrupted)
			if (stream->audioBufEmpty[0]) {
				fillStreamingBuffer(stream, 0);
			}
			if (stream->audioBufEmpty[1]) {
				fillStreamingBuffer(stream, 1);
			}
		}
	}
}
