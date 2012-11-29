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
// Name        : WinSound.h
// Description : Windows sound init and callback
//============================================================================

#ifndef	WINSOUND_H_
#define	WINSOUND_H_

#define	WINSOUND_RATE 44100
#define WINSOUND_CHANNELS 2
#define	WINSOUND_CHANNEL_DEPTH 16
#define	WINSOUND_SAMPLE_BYTES (WINSOUND_CHANNEL_DEPTH/8)
#define	WINSOUND_BUFFERCOUNT 2
// total buffer length in ms (doesn't work well under 100)
#define WINSOUND_BUFFERLEN 100

typedef void (*USER_CALLBACK) (void *pBuffer,long bufferLen);

class WinSound {
	public:
			WinSound();
			~WinSound();
			BOOL start(USER_CALLBACK pUserCallback);
			void stop(void);
			void fillNextBuffer(void);
	private:
			WAVEHDR waveHeader[WINSOUND_BUFFERCOUNT];
			void *soundBuffer[WINSOUND_BUFFERCOUNT];
			BOOL isStarted;
			BOOL isFilling;
			HWND hWnd;
			long bufferSize;
			long currentBuffer;
			HWAVEOUT waveOut;
			USER_CALLBACK userCallback;
};

#endif
