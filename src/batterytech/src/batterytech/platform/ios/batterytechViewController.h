
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
// Name        : batterytechViewController.h
// Description : Primary driver class for Batterytech on IOS
//============================================================================

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#include <CoreFoundation/CFURL.h>

#import "RemoteIOPlayer.h"

#define MAX_TOUCHES 10

@interface batterytechViewController : UIViewController <UIAccelerometerDelegate> {
    EAGLContext *context;
     
    BOOL animating;
    BOOL isContextInitialized;
    BOOL displayLinkSupported;
    NSInteger animationFrameInterval;
    /*
	 Use of the CADisplayLink class is the preferred method for controlling your animation timing.
	 CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
	 The NSTimer object is used only as fallback when running on a pre-3.1 device where CADisplayLink isn't available.
	 */
    id displayLink;
    NSThread *animationThread;
	RemoteIOPlayer *player;
	UITouch **touchIds;
    int vpWidth;
    int vpHeight;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic, retain) RemoteIOPlayer *player;
@property (nonatomic, retain) EAGLContext *context;

- (void)startAnimation;
- (void)stopAnimation;
- (void)pauseBTApplication;
- (void)resumeBTApplication;
- (void)runMainLoop;
- (void)drawFrame;
// now for hooks and customizations
- (void) showAd;
- (void) hideAd;
- (void) hook:(const char*)hookData withResult:(char*) result withResultLen:(int) resultLen;
- (bool) useGLES2;
- (bool) forceLandscape;
// Calls back into BatteryTech, waiting for an open slot.  Do not call this from the game thread or you may loop forever!
- (void) callback:(const char*)data;
@end

#endif
