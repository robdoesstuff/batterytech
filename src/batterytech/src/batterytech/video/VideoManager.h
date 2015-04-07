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
// Name        : VideoManager.h
// Description : Tie-ins for video playback - experimental
//============================================================================

#include "../batterytech_globals.h"

#include "btFFMpegPlayer.h"
#include "../primitives.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

namespace BatteryTech {
	class Context;
	class VideoManager {
		public:
			VideoManager(Context *context);
			virtual ~VideoManager();
			void load(const char *assetName);
			void play(BOOL32 loop=FALSE);
			void pause();
			void stop();
			void unload();
			void update(F32 delta);
			void render();
			void addAudioBuffer(void *pSoundBuffer, long bufferLen);
			F32 getPosition();
		private:
			Context *context;
#ifdef WITH_FFMPEG
#if _WIN32 || (TARGET_OS_MAC && !TARGET_OS_IPHONE)
			btFFMpegPlayer *ffmpegPlayer;
#endif
#endif
	};

}
