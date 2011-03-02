//
//  EAGLView.h
//  batterytech-ios
//
//  Created by Apple on 10/17/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/glu.h>

#import "RemoteIOPlayer.h"

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface BatterytechGLView : NSOpenGLView {
@private

}

NSTimer* timer;
RemoteIOPlayer *player;

- (id) initWithFrame: (NSRect) frameRect;
- (void) awakeFromNib;

- (void)animationTimer:(NSTimer *)timer;

- (void)keyDown:(NSEvent *)theEvent;

- (void) mouseDown:(NSEvent *)theEvent;
- (void) mouseUp:(NSEvent *)theEvent;
- (void) mouseDragged:(NSEvent *)theEvent;

- (void) update;		// moved or resized

- (BOOL) acceptsFirstResponder;
- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;

@property (nonatomic, retain) RemoteIOPlayer *player;

@end
