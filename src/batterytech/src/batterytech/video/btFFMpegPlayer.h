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
// Name        : btFFMpegPlayer.h
// Description : Experimental FFMpeg support - not production ready
//============================================================================

#ifndef	BTFFMPEGPLAYER_H_
#define	BTFFMPEGPLAYER_H_

#include "../batterytech_globals.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef WITH_FFMPEG 
#if _WIN32 || (TARGET_OS_MAC && !TARGET_OS_IPHONE)

#include "../platform/platformgl.h"
#include "../primitives.h"
#include "../util/RingBuffer.h"

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

namespace BatteryTech { class Context; }

class btFFMpegPlayer {
	enum PlayerState { STATE_NONE, STATE_LOADED, STATE_PLAYING, STATE_PAUSED, STATE_ENDED };
	public:
		btFFMpegPlayer(Context *context, const char *assetName);
		virtual ~btFFMpegPlayer();
		void renderFrame();
		void addAudioBuffer(void *pSoundBuffer, long bufferLen);
		void update(F32 delta);
		void play(BOOL32 loop);
		void stop();
		void pause();
		F32 getPosition();
		BOOL32 isPaused();
		BOOL32 isPlaying();
		BOOL32 isEnded();
	private:
		void initializePlayer();
		void initializeGL();
		void decodeFrame();
		Context *context;
		char *assetName;
		BOOL32 initialized;
	    AVFormatContext *pFormatCtx;
	    int             videoStream;
	    int				audioStream;
	    AVCodecContext  *videoCodecContext;
	    AVCodec         *videoCodec;
	    AVCodecContext  *audioCodecContext;
	    AVCodec         *audioCodec;
	    AVFrame         *audioFrame;
    RingBuffer<AVFrame> *audioRingBuffer;
        S32             audioBytesLeft;
        long 			frameBytesUsed;
	    AVFrame         *videoFrame;
	    AVFrame         *videoFrameRGB;
	    AVPacket        packet;
	    int             frameFinished;
	    uint8_t         *videoBuffer;
        uint8_t         *audioBuffer;
        int             audioBufferSize;
        bool            audioReady;
	    GLuint			textureId;
	    F32				position;
	    F32				vidFramePosition;
	    PlayerState		state;
};

#endif
#endif

#endif
