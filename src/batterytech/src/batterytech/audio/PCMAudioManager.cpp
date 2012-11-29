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
// Name        : PCMAudioManager.cpp
// Description : Low-level PCM Manager for audio playback using only a platform PCM Stream
// Usage       : Do not use directly.  Use AudioManager.  See PREFER_PLATFORM_AUDIO_MANAGEMENT
//             : in batterytech_globals.h to force using this over using platform services.
//============================================================================

#include "PCMAudioManager.h"
#include "../Logger.h"
#include "../decoders/stb_vorbis.h"
#include "../platform/platformgeneral.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "../batterytech_globals.h"
#include "../batterytech.h"
#include "../Context.h"
#include "../util/strx.h"

struct stb_vorbis;

namespace BatteryTech {

	//FILE *testOutFile;

	PCMAudioManager::PCMAudioManager() {
		isReady = FALSE;
		lastStreamId = 0;
		lastSoundId = 0;
		streamCount = 0;
		pcmStreams = NULL;
		pcmSounds = NULL;
	}

	void PCMAudioManager::init(U8 streams) {
		streamCount = streams;
		pcmSounds = new ManagedArray<PCMSound>(MAX_PCM_SOUNDS);
		pcmStreams = new PCMStream[streams];
		isReady = TRUE;
		//testOutFile = fopen("output.wav", "wb");
	}


	PCMAudioManager::~PCMAudioManager() {
		logmsg("Releasing PCMSoundManager");
		//fclose(testOutFile);
		pcmSounds->deleteElements();
		delete pcmSounds;
		delete [] pcmStreams;
		pcmSounds = NULL;
		pcmStreams = NULL;
		isReady = FALSE;
		streamCount = 0;
	}

	S32 PCMAudioManager::loadSound(const char *assetName) {
		Property *prop = btGetContext()->appProperties->get("debug_sounds");
		BOOL32 debugSounds = FALSE;
		if (prop) {
			debugSounds = prop->getBoolValue();
		}
        if (isSoundLoaded(assetName)) {
            return getSoundId(assetName);
        }
		int assetSize = 0;
		unsigned char *fileData = _platform_load_asset(assetName, &assetSize);
		S32 sndId = -1;
		if (fileData) {
			signed short *decoded;
			int channels, len;
			unsigned int sampleRate = 0;
			len = stb_vorbis_decode_memory(fileData, assetSize, &channels, &sampleRate, &decoded);
			if (debugSounds) {
				char buf[1024];
				sprintf(buf, "Loaded %s: (%i enc bytes) length=%i channels=%i rate=%i", assetName, assetSize, len, channels, sampleRate);
				logmsg(buf);
			}
			sndId = loadSound(decoded, len * channels, sampleRate, channels, assetName);
		}
		_platform_free_asset(fileData);
		return sndId;
	}

	S32 PCMAudioManager::loadSound(S16 *pcmData, U32 length, U32 rate, U8 channels, const char *assetName) {
		PCMSound *pcmSound = new PCMSound(pcmData, length, rate, channels, assetName, lastSoundId++);
		pcmSounds->add(pcmSound);
		return pcmSound->soundId;
	}

	S32 PCMAudioManager::playSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
		//logmsg("Playsound");
		//logmsg(assetName);
		return playSound(getSoundId(assetName), loops, leftVol, rightVol, rate);
	}

    BOOL32 PCMAudioManager::isSoundLoaded(const char *assetName) {
		for (S32 i = 0; i < pcmSounds->getSize(); i++) {
			if (strcmp(pcmSounds->array[i]->assetName, assetName) == 0) {
				return TRUE;
			}
		}
        return FALSE;
    }
    
	S32 PCMAudioManager::getSoundId(const char *assetName) {
		for (S32 i = 0; i < pcmSounds->getSize(); i++) {
			if (strcmp(pcmSounds->array[i]->assetName, assetName) == 0) {
				return pcmSounds->array[i]->soundId;
			}
		}
		char buf[50];
		sprintf(buf, "Sound %s not loaded", assetName);
		logmsg(buf);
		return -1;
	}

	PCMSound* PCMAudioManager::getSound(S32 soundId) {
		for (S32 i = 0; i < pcmSounds->getSize(); i++) {
			if (pcmSounds->array[i]->soundId == soundId) {
				return pcmSounds->array[i];
			}
		}
		return NULL;
	}

	S32 PCMAudioManager::playSound(S32 soundId, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
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
			if (!stream->isPlaying && !stream->audioHandle) {
				//sprintf(buf, "Using Stream %i", i);
				//logmsg(buf);
				// use this stream;
				stream->isStreaming = FALSE;
				PCMSound *sound = getSound(soundId);
				stream->pcmSound = sound;
				stream->loopsRemaining = loops;
				stream->playbackRate = rate;
				stream->bufferPosition = 0;
				stream->overallPosition = 0;
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
				stream->resampleInt = stream->resampleActiveRate / PCM_AUDIO_PLAYBACK_RATE;
				stream->resampleFrac = stream->resampleActiveRate % PCM_AUDIO_PLAYBACK_RATE;
				stream->resampleFracAccumulated = 0;
				stream->filePosition = 0;
				stream->dataBuf = 0;
				stream->isPlaying = TRUE;
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

	void PCMAudioManager::stopAllSounds() {
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

	void PCMAudioManager::stopSound(S32 streamId) {
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

	void PCMAudioManager::stopSound(const char *assetName) {
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

	S32 PCMAudioManager::playStreamingSound(const char *assetName, S16 loops, F32 leftVol, F32 rightVol, F32 rate) {
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
			if (!stream->isPlaying && !stream->audioHandle) {
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
				stream->assetName = strDuplicate(assetName);
				stream->channels = 0;
				stream->playbackRate = rate;
				stream->filePosition = 0;
				stream->assetLength = _platform_get_asset_length(assetName);
				if (stream->assetLength > 0) {
					// get vorbis handle and read info
					// fill both audio buffers
					if (!stream->audioBuf) {
						stream->audioBuf = new S16*[PCM_AUDIO_STREAM_BUFFERS];
						for (S32 j = 0; j < PCM_AUDIO_STREAM_BUFFERS; j++) {
							stream->audioBuf[j] = new S16[PCM_AUDIO_STREAM_BUFFER_SIZE];
						}
					}
					unsigned char buf[CHUNKED_READ_BUFFER_SIZE];
					BOOL32 eof = FALSE;
					S32 bytesRead = _platform_read_asset_chunk(assetName, 0, buf, CHUNKED_READ_BUFFER_SIZE, &eof);
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
					stream->resampleInt = stream->resampleActiveRate / PCM_AUDIO_PLAYBACK_RATE;
					stream->resampleFrac = stream->resampleActiveRate % PCM_AUDIO_PLAYBACK_RATE;
					stream->resampleFracAccumulated = 0;
					for (S32 j = 0; j < PCM_AUDIO_STREAM_BUFFERS; j++) {
						fillStreamingBuffer(stream, j);
					}
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

	void PCMAudioManager::setLoops(S32 streamId, S16 loops) {
		for (S32 i = 0; i < streamCount; i++) {
			PCMStream *stream = &pcmStreams[i];
			if (stream->streamId == streamId) {
				stream->loopsRemaining = loops;
			}
		}
	}

	void PCMAudioManager::setVolume(S32 streamId, F32 leftVol, F32 rightVol) {
		for (S32 i = 0; i < streamCount; i++) {
			PCMStream *stream = &pcmStreams[i];
			if (stream->streamId == streamId) {
				leftVol = leftVol < 0 ? 0 : leftVol;
				leftVol = leftVol > 1 ? 1 : leftVol;
				rightVol = rightVol < 0 ? 0 : rightVol;
				rightVol = rightVol > 1 ? 1 : rightVol;
				stream->volumeLeft = leftVol;
				stream->volumeRight = rightVol;
			}
		}
	}

	void PCMAudioManager::setRate(S32 streamId, F32 rate) {
		for (S32 i = 0; i < streamCount; i++) {
			PCMStream *stream = &pcmStreams[i];
			if (stream->streamId == streamId) {
				rate = rate < 0.5f ? 0.5f : rate;
				rate = rate > 2.0f ? 2.0f : rate;
				stream->playbackRate = rate;
				stream->resampleActiveRate = stream->sampleRate * stream->playbackRate;
				stream->resampleInt = stream->resampleActiveRate / PCM_AUDIO_PLAYBACK_RATE;
				stream->resampleFrac = stream->resampleActiveRate % PCM_AUDIO_PLAYBACK_RATE;
			}
		}
	}

	void PCMAudioManager::stopStreamingSound(const char *assetName) {
		for (S32 i = 0; i < streamCount; i++) {
			PCMStream *stream = &pcmStreams[i];
			if (stream->isPlaying && stream->isStreaming && strcmp(assetName, stream->assetName) == 0) {
				stream->isPlaying = FALSE;
				stream->bufferPosition = 0;
				stream->overallPosition = 0;
				stream->loopsRemaining = 0;
				if (stream->audioHandle) {
					stb_vorbis_close((stb_vorbis*)stream->audioHandle);
					stream->audioHandle = NULL;
				}
                // we previously allocated the name in playStreamingSound
                delete [] stream->assetName;
                stream->assetName = NULL;
			}
		}
	}

	void PCMAudioManager::fillStreamingBuffer(PCMStream *stream, U16 bufNum) {
		BOOL32 done = FALSE;
		U32 audioBufPos = 0;
		// char cBuf[50];
		// sprintf(cBuf, "Filling bufNum %d", bufNum);
		// logmsg(cBuf);
		// U32 startingFilePos = stream->filePosition;
		while (!done) {
			// read data
			unsigned char buf[CHUNKED_READ_BUFFER_SIZE];
			// sprintf(cBuf, "Reading at %d", stream->filePosition);
			// logmsg(cBuf);
			BOOL32 eof = FALSE;
			S32 bytesRead = _platform_read_asset_chunk(stream->assetName, stream->filePosition, buf, CHUNKED_READ_BUFFER_SIZE, &eof);
            if (bytesRead == 0) {
                // can't decode 0 bytes!  The file must not be available.
                logmsg("Stream asset unavailable - stopping playback");
                stream->isPlaying = FALSE;
                return;
            }
            if (stream->filePosition == 0) {
            	// starting loop over, must flush or some audio files get stuck
            	stb_vorbis_flush_pushdata((stb_vorbis*) stream->audioHandle);
            }
			S32 bytesConsumed = 0;
			// fill up audio buffer
			F32 **audioBuf;
			S32 samplesDecoded = 0;
			S32 channelsUsed = 0;
			BOOL32 hasMoreData = TRUE;
			S16 *streamAudioBuf = stream->audioBuf[bufNum];
			while (hasMoreData && audioBufPos + (VORBIS_MAX_LEGAL_FRAME * stream->channels) < PCM_AUDIO_STREAM_BUFFER_SIZE) {
				S32 dBytesConsumed = stb_vorbis_decode_frame_pushdata((stb_vorbis*) stream->audioHandle, buf + bytesConsumed, bytesRead - bytesConsumed, &channelsUsed, &audioBuf, &samplesDecoded);
				if (dBytesConsumed < 0) {
					logmsg("vorbis decoder error");
					hasMoreData = FALSE;
				} else if (dBytesConsumed == 0 && samplesDecoded == 0) {
					hasMoreData = FALSE;
					// logmsg("no bytes consumed or samples decoded");
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
						stream->tempLength += samplesDecoded * channelsUsed;
					}
					audioBufPos += samplesDecoded * channelsUsed;
				} else {
					bytesConsumed += dBytesConsumed;
				}
			}
			stream->audioBufLimit[bufNum] = audioBufPos;
			if (eof) {
				stream->filePosition = 0;
			} else {
				stream->filePosition += bytesConsumed;
				stream->filePosition %= stream->assetLength;
			}
			stream->audioBufEmpty[bufNum] = FALSE;
			if (hasMoreData) {
				// buffer is full
				done = TRUE;
                // TODO - oops, what was this supposed to do?
				//if (stream->filePosition < 0) {
				//	stream->filePosition = startingFilePos + bytesConsumed;
				//}
			}
		}
	}

	void PCMAudioManager::unloadSound(const char *assetName) {
		S32 soundId = getSoundId(assetName);
		unloadSound(soundId);
	}

	void PCMAudioManager::unloadSound(S32 soundId) {
		if (soundId != -1) {
			PCMSound *sound = getSound(soundId);
			if (sound) {
				pcmSounds->remove(sound);
				delete sound;
			}
		}
	}

	void PCMAudioManager::fillBuffer(void *pSoundBuffer, long bufferLen) {
		//logmsg("fillBuffer");
		U8 playbackChannels = PCM_AUDIO_PLAYBACK_CHANNELS;
		U16 playbackRate = PCM_AUDIO_PLAYBACK_RATE;
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
				U32 bufferEnd;
				U32 streamEnd;
				BOOL32 isStreaming = stream->isStreaming;
				U8 activeAudioBuf = stream->activeAudioBuf;
				U8 sourceChannels = stream->channels;
				S32 resampleInt = stream->resampleInt;
				S32 resampleFrac = stream->resampleFrac;
				U32 bufferPosition = stream->bufferPosition;
				U32 overallPosition = stream->overallPosition;
				U32 resampleFracAccumulated = stream->resampleFracAccumulated;
				S16 *sampleSource;
				if (isStreaming) {
					sampleSource = stream->audioBuf[activeAudioBuf];
				} else {
					sampleSource = pcmSound->pcmData;
				}
				if (isStreaming) {
					bufferEnd = stream->audioBufLimit[activeAudioBuf];
					streamEnd = stream->length;
					BOOL32 hasData = FALSE;
					for (S32 j = 0; j < PCM_AUDIO_STREAM_BUFFERS; j++) {
						if (!stream->audioBufEmpty[j]) {
							hasData = TRUE;
							break;
						}
					}
					if (!hasData) {
						// underrun, wait for fill
						continue;
					}
				} else {
					streamEnd = bufferEnd = pcmSound->length;
				}
				// **** entering super performance-critical code section ****
				for (j=0; j<nbSample; j += playbackChannels) {
					// mix with 32 bit headroom
					S32 mix;
					// for each output channel (interleaved)
					if (sourceChannels == playbackChannels) {
						// mono to mono or stereo to stereo
						for (channelIdx = 0; channelIdx < sourceChannels; channelIdx++) {
							mix = pSample[j + channelIdx] + (sampleSource[bufferPosition + channelIdx] * volumes[channelIdx]);
							if ((U32) (mix + 32768) > 65535) {
								mix = mix < 0 ? -32767 : 32767;
							}
							pSample[j + channelIdx] = (S16) mix;
						}
						// left
						// right
					} else if (sourceChannels == 1) {
						// mono to stereo
						// left
						mix = pSample[j] + (sampleSource[bufferPosition] * volumes[0]);
						if ((U32) (mix + 32768) > 65535) {
							mix = mix < 0 ? -32767 : 32767;
						}
						pSample[j] = (S16) mix;
						// right
						mix = pSample[j+1] + (sampleSource[bufferPosition] * volumes[1]);
						if ((U32) (mix + 32768) > 65535) {
							mix = mix < 0 ? -32767 : 32767;
						}
						pSample[j+1] = (S16) mix;
					} else {
						// stereo to mono
						mix = pSample[j] + (sampleSource[bufferPosition] * volumes[0] * 0.5f) + (sampleSource[bufferPosition+1] * volumes[1] * 0.5f);
						if ((U32) (mix + 32768) > 65535) {
							mix = mix < 0 ? -32767 : 32767;
						}
						pSample[j] = (S16) mix;
					}
					// update source position
					bufferPosition += resampleInt * sourceChannels;
					overallPosition += resampleInt * sourceChannels;
					resampleFracAccumulated += resampleFrac;
					if (resampleFracAccumulated >= playbackRate) {
						resampleFracAccumulated -= playbackRate;
						bufferPosition += sourceChannels;
						overallPosition += sourceChannels;
					}
					if (bufferPosition >= bufferEnd) {
						bufferPosition = 0;
						if (isStreaming) {
							stream->audioBufEmpty[activeAudioBuf] = TRUE;
							S32 oldActiveAudioBuf = activeAudioBuf++;
							while (activeAudioBuf != oldActiveAudioBuf) {
								activeAudioBuf %= PCM_AUDIO_STREAM_BUFFERS;
								if (stream->audioBufEmpty[activeAudioBuf]) {
									activeAudioBuf++;
								} else {
									// this is the next buffer that's not full
									stream->activeAudioBuf = activeAudioBuf;
									sampleSource = stream->audioBuf[activeAudioBuf];
									bufferEnd = stream->audioBufLimit[activeAudioBuf];
									break;
								}
							}
							if (activeAudioBuf == oldActiveAudioBuf) {
								// no buffers had any data :(
								continue;
							}
						}
					}
					if (streamEnd && overallPosition >= streamEnd) {
						overallPosition = 0;
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
				// update stream's members
				stream->bufferPosition = bufferPosition;
				stream->overallPosition = overallPosition;
				stream->resampleFracAccumulated = resampleFracAccumulated;
				//char cBuf[50];
				//sprintf(cBuf, "buf %d at %d of %d %d/%d", activeAudioBuf, stream->bufferPosition, bufferEnd, stream->overallPosition, streamEnd);
				//logmsg(cBuf);
			}
		}
		//fwrite(pSample, sizeof(S16), nbSample, testOutFile);
	}

	void PCMAudioManager::update() {
		// check streams
		for (S32 i = 0; i < streamCount; i++) {
			PCMStream *stream = &pcmStreams[i];
			if (stream->isPlaying && stream->isStreaming) {
				//fill empty buffers (will normally only be 1 to fill unless app was interrupted)
				for (S32 j = 0; j < PCM_AUDIO_STREAM_BUFFERS; j++) {
					if (stream->audioBufEmpty[j]) {
						fillStreamingBuffer(stream, j);
					}
				}
			}
			// clean up any finished streams
			if (!stream->isPlaying && stream->audioHandle) {
				stb_vorbis_close((stb_vorbis*)stream->audioHandle);
				stream->audioHandle = NULL;
			}
		}
	}

}
