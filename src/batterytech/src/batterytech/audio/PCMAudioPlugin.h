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
// Name        : PCMAudioPlugin.h
// Description : An interface for adding PCM Audio streams to your BT application
//============================================================================

#ifndef PCMAUDIOPLUGIN_H_
#define PCMAUDIOPLUGIN_H_

namespace BatteryTech {

class PCMAudioPlugin {
public:
	virtual ~PCMAudioPlugin() {};
	/** \brief offers up the opportunity for the PCMAudioPlugin to add data to the buffer
	 *
	 * The buffer already will have audio data, so in order to work nicely with the existing system,
	 * each sample should be added to the existing one in the buffer - or should modify the existing sample to provide an effect.
	 *
	 * The plugin should not blindly replace the data in the buffer without taking it into consideration first, unless of course
	 * the goal is to provide a completely independent audio chain that hijacks the audio stream.
	 *
	 * \param buffer A buffer (usually of S16 samples, stereo, 44khz, interleaved)
	 * \param numBytes The number of bytes in this buffer
	 */
	virtual void addToBuffer(void *buffer, long numBytes)=0;
};

}

#endif /* PCMAUDIOPLUGIN_H_ */
