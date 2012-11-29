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
// Name        : EAGLView.m
// Description : Wraps GLES EGL for IOS - handles init and swap
//============================================================================

#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@dynamic context;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
    NSLog(@"EAGLView initWithCoder");
    framebufferWidth = 0;
    framebufferHeight = 0;
    defaultFramebuffer = 0;
    colorRenderbuffer = 0;
    depthRenderbuffer = 0;
    self = [super initWithCoder:coder];
	if (self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
    }
	[self configureScale];
    return self;
}

- (void) configureScale {
	CGFloat screenScale;
	if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
		//iphones with the latest SDK should end up in here (and also ipads when they are updated)
		NSMethodSignature * scaleSignature = [UIScreen instanceMethodSignatureForSelector:@selector(scale)];
		NSInvocation * scaleInvocation = [NSInvocation invocationWithMethodSignature:scaleSignature];
		[scaleInvocation setTarget:[UIScreen mainScreen]];
		[scaleInvocation setSelector:@selector(scale)];
		[scaleInvocation invoke];
		
		NSInteger returnLength = [[scaleInvocation methodSignature] methodReturnLength];
		//good memory management to check this in case anything changed in the future
		if (returnLength == sizeof(CGFloat)) {
			[scaleInvocation getReturnValue:&screenScale];
		} else {
			//default value
			screenScale = 1.0f;
		}
	} else {
		//ipad (for now) and other SDK < 4.0 should come here
		screenScale = 1.0f;
	}
	if ([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")]) {
		[self setContentScaleFactor:screenScale];
	} 
	[self setScaleFactor:screenScale];
}

- (void)dealloc
{
    [self deleteFramebuffer];    
    [context release];
    
    [super dealloc];
}

- (EAGLContext *)context
{
    return context;
}

- (void)setContext:(EAGLContext *)newContext
{
    if (context != newContext)
    {
        [self deleteFramebuffer];
        
        [context release];
        context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (float)scaleFactor {
	return scaleFactor;
}

- (void)setScaleFactor:(float)newScaleFactor {
	scaleFactor = newScaleFactor;
}

- (void)createFramebuffer
{
    @synchronized(self) {
        if (context && !defaultFramebuffer)
        {
            if (![EAGLContext setCurrentContext:context]) {
                NSLog(@"*** in createFramebuffer - setCurrentContext failed!");
            }
             
            // Create default framebuffer object.
            glGenFramebuffers(1, &defaultFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
            
            // Create color render buffer and allocate backing store.
            glGenRenderbuffers(1, &colorRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
            [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
            
            // depth buffer
            glGenRenderbuffers(1, &depthRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
            
            NSLog(@"Creating a frame buffer of size %dx%d", framebufferWidth, framebufferHeight);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
                [self deleteFramebuffer];
            }
        }
    }
}

- (void)deleteFramebuffer
{
    @synchronized(self) {
        if (context) {
            [EAGLContext setCurrentContext:context];
            if (defaultFramebuffer) {
                NSLog(@"Deleting framebuffer %d", defaultFramebuffer);
                glDeleteFramebuffers(1, &defaultFramebuffer);
                defaultFramebuffer = 0;
            }
            if (colorRenderbuffer) {
                NSLog(@"Deleting color renderbuffer %d", colorRenderbuffer);
                glDeleteRenderbuffers(1, &colorRenderbuffer);
                colorRenderbuffer = 0;
            }
            if (depthRenderbuffer) {
                NSLog(@"Deleting depth renderbuffer %d", depthRenderbuffer);
                glDeleteRenderbuffers(1, &depthRenderbuffer);
                depthRenderbuffer = 0;
            }
        }
    }
}

- (void)setFramebuffer
{
    @synchronized(self) {
        if (context)
        {
            [EAGLContext setCurrentContext:context];
            if (!defaultFramebuffer) {
                [self createFramebuffer];
            }
            if (defaultFramebuffer) {
                glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
                glViewport(0, 0, framebufferWidth, framebufferHeight);
            }
        }
    }
}

- (BOOL)presentFramebuffer
{
    @synchronized(self) {
        BOOL success = FALSE;
        if (context) {
            [EAGLContext setCurrentContext:context];
            if (colorRenderbuffer) {
                glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
                success = [context presentRenderbuffer:GL_RENDERBUFFER];
            }
        }
        if (!success) { NSLog(@"presentFramebuffer failed"); };
        return success;
    }
}

- (GLint)getFBWidth {
	return framebufferWidth;
}

- (GLint)getFBHeight {
	return framebufferHeight;
}

- (void)layoutSubviews
{
	[self configureScale];
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
}

@end

#endif
