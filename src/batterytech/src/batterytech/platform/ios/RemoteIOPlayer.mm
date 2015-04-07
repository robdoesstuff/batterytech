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
// Name        : RemoteIOPlayer.mm
// Description : Hooks IOS AudioUnit up to PCMSoundManager
//============================================================================

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

#import "RemoteIOPlayer.h"
#include <AudioUnit/AudioUnit.h>
#define kOutputBus 0
#define kInputBus 1
#include <batterytech/audio/AudioManager.h>

using namespace BatteryTech;

extern AudioManager *_iosSndMgr;

@implementation RemoteIOPlayer

AudioComponentInstance audioUnit;
AudioStreamBasicDescription audioFormat;

-(OSStatus)start{
	
	OSStatus status = AudioOutputUnitStart(audioUnit);
	return status;
}

-(OSStatus)stop{
	OSStatus status = AudioOutputUnitStop(audioUnit);
	return status;
}

-(void)cleanUp{
	AudioUnitUninitialize(audioUnit);
}


static OSStatus playbackCallback(void *inRefCon, 
								 AudioUnitRenderActionFlags *ioActionFlags, 
								 const AudioTimeStamp *inTimeStamp, 
								 UInt32 inBusNumber, 
								 UInt32 inNumberFrames, 
								 AudioBufferList *ioData) {  
	//loop through all the buffers that need to be filled
	for (int i = 0 ; i < ioData->mNumberBuffers; i++){
		//get the buffer to be filled
		AudioBuffer buffer = ioData->mBuffers[i];
		// 4 bytes per frame for 16 bit stereo
		UInt16 frames = inNumberFrames * 4;
		_iosSndMgr->fillBuffer((UInt16*)buffer.mData, frames);
	}
    return noErr;
}

-(void)initialiseAudio{
	OSStatus status;
	
	
	// Describe audio component
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	
	// Get component
	AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
	
	// Get audio units
	status = AudioComponentInstanceNew(inputComponent, &audioUnit);
	
	UInt32 flag = 1;
	// Enable IO for playback
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioOutputUnitProperty_EnableIO, 
								  kAudioUnitScope_Output, 
								  kOutputBus,
								  &flag, 
								  sizeof(flag));
	
	// Describe format
	audioFormat.mSampleRate			= 44100.00;
	audioFormat.mFormatID			= kAudioFormatLinearPCM;
	audioFormat.mFormatFlags		= kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
	audioFormat.mFramesPerPacket	= 1;
	audioFormat.mChannelsPerFrame	= 2;
	audioFormat.mBitsPerChannel		= 16;
	audioFormat.mBytesPerPacket		= 4;
	audioFormat.mBytesPerFrame		= 4;
	
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioUnitProperty_StreamFormat, 
								  kAudioUnitScope_Input, 
								  kOutputBus, 
								  &audioFormat, 
								  sizeof(audioFormat));
	 
	AURenderCallbackStruct callbackStruct;
	callbackStruct.inputProc = playbackCallback;
	//set the reference to "self" this becomes *inRefCon in the playback callback
	callbackStruct.inputProcRefCon = self;
	
	status = AudioUnitSetProperty(audioUnit, 
								  kAudioUnitProperty_SetRenderCallback, 
								  kAudioUnitScope_Global, 
								  kOutputBus,
								  &callbackStruct, 
								  sizeof(callbackStruct));
	
	status = AudioUnitInitialize(audioUnit);
}

@end

#endif