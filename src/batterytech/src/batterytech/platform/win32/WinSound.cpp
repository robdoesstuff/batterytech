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
// Name        : WinSound.cpp
// Description : Windows sound init and callback
//============================================================================

#ifdef _WIN32

#include <windows.h>
#include "WinSound.h"

// WaveOut API callback function
static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	if (uMsg == WOM_DONE) {
		WinSound *winSound = (WinSound*) dwInstance;
		if (winSound) {
			winSound->fillNextBuffer();
		}
	}
}

WinSound::WinSound() {
	userCallback = NULL;
	isStarted = FALSE;
	currentBuffer = 0;
	isFilling = FALSE;
}

WinSound::~WinSound() {
	stop();
}

BOOL WinSound::start(USER_CALLBACK pUserCallback) {
	userCallback = pUserCallback;
	// bufferSize is number of bytes per buffer
	bufferSize = (((WINSOUND_BUFFERLEN * WINSOUND_RATE) / 1000) * WINSOUND_SAMPLE_BYTES * WINSOUND_CHANNELS) / WINSOUND_BUFFERCOUNT;
	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = WINSOUND_CHANNELS;
	wfx.nSamplesPerSec = WINSOUND_RATE;
	wfx.wBitsPerSample = WINSOUND_CHANNEL_DEPTH;
	wfx.nBlockAlign = wfx.nChannels * WINSOUND_SAMPLE_BYTES;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign + wfx.nSamplesPerSec;
	wfx.cbSize = 0;
	MMRESULT errCode = waveOutOpen(&waveOut, WAVE_MAPPER, &wfx, (DWORD) waveOutProc, (DWORD) this, (DWORD) CALLBACK_FUNCTION);
	if (errCode != MMSYSERR_NOERROR) {
		return FALSE;
	}
	int i;
	for (i = 0; i < WINSOUND_BUFFERCOUNT; i++) {
		soundBuffer[i] = malloc(bufferSize);
		memset(&waveHeader[i], 0, sizeof(WAVEHDR));
	}
	// Fill all the sound buffers
	currentBuffer = 0;
	isStarted = TRUE;
	for (i = 0; i < WINSOUND_BUFFERCOUNT; i++) {
		fillNextBuffer();
	}
	return TRUE;
}

void WinSound::stop(void) {
	if (isStarted) {
		isStarted = FALSE;
		while (isFilling) {
			Sleep(1);
		}
		waveOutReset(waveOut);
		userCallback = NULL;
		for (int i = 0; i < WINSOUND_BUFFERCOUNT; i++) {
			if (waveHeader[currentBuffer].dwFlags & WHDR_PREPARED) {
				waveOutUnprepareHeader(waveOut, &waveHeader[i],
						sizeof(WAVEHDR));
			}
			// TODO - why segfault?
			//free(soundBuffer[i]);
		}
		waveOutClose(waveOut);
	}
}

void WinSound::fillNextBuffer(void) {
	if (isStarted) {
		isFilling = TRUE;
		// check if the buffer is already prepared (should not !)
		if (waveHeader[currentBuffer].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(waveOut, &waveHeader[currentBuffer],
					sizeof(WAVEHDR));
		// Call the user function to fill the buffer
		if (userCallback) {
			userCallback(soundBuffer[currentBuffer], bufferSize);
		}
		// Prepare the buffer to be sent to the WaveOut API
		waveHeader[currentBuffer].lpData = (char*) soundBuffer[currentBuffer];
		waveHeader[currentBuffer].dwBufferLength = bufferSize;
		waveOutPrepareHeader(waveOut, &waveHeader[currentBuffer], sizeof(WAVEHDR));
		// Send the buffer the the WaveOut queue
		waveOutWrite(waveOut, &waveHeader[currentBuffer], sizeof(WAVEHDR));
		currentBuffer++;
		if (currentBuffer >= WINSOUND_BUFFERCOUNT) {
			currentBuffer = 0;
		}
		isFilling = FALSE;
	}
}

#endif
