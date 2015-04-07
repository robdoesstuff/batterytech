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
// Name        : batterytechGLView.h
// Description : Primary driver for Batterytech on OSX
//============================================================================

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_MAC && !TARGET_OS_IPHONE

#import "RemoteIOPlayer.h"
#import <Cocoa/Cocoa.h>

@interface BatterytechGLView : NSOpenGLView {
NSTimer* timer;
RemoteIOPlayer *player;
}

- (id) initWithFrame: (NSRect) frameRect;
- (void) awakeFromNib;

- (void)animationTimer:(NSTimer *)timer;

- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;

- (void) mouseDown:(NSEvent *)theEvent;
- (void) mouseUp:(NSEvent *)theEvent;
- (void) mouseDragged:(NSEvent *)theEvent;

- (void) update;		// moved or resized

- (BOOL) acceptsFirstResponder;
- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;

@property (nonatomic, retain) RemoteIOPlayer *player;

@end

#endif