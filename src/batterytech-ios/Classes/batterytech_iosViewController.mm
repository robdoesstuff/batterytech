//
//  batterytech_iosViewController.m
//  batterytech-ios
//
//  Created by Apple on 10/17/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

#import "batterytech_iosViewController.h"
#import "EAGLView.h"
#include "../../batterytech/src/batterytech/batterytech.h"
#include "../../batterytech/src/batterytech/render/GraphicsConfiguration.h"

static GraphicsConfiguration *gConfig;
static double currentTime;
static double lastTime;
double getCurrentTime();

@interface batterytech_iosViewController ()
@property (nonatomic, retain) EAGLContext *context;
@end

@implementation batterytech_iosViewController

@synthesize animating, context;

- (void)awakeFromNib
{
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];    
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
        
    animating = FALSE;
    displayLinkSupported = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
    animationTimer = nil;
    
    // Use of CADisplayLink requires iOS version 3.1 or greater.
	// The NSTimer object is used as fallback when it isn't available.
    NSString *reqSysVer = @"3.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
        displayLinkSupported = TRUE;
	
	gConfig = new GraphicsConfiguration;
	gConfig->supportsHWmipmapgen = TRUE;
	gConfig->supportsVBOs = TRUE;
	gConfig->supportsUVTransform = TRUE;
	// TODO - why do we need to reverse these for landscape?? Should be width, height
	btInit(gConfig, [(EAGLView *)self.view getFBHeight], [(EAGLView *)self.view getFBWidth]);
	currentTime = getCurrentTime();
}

- (void)dealloc
{
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        if (displayLinkSupported)
        {
            /*
			 CADisplayLink is API new in iOS 3.1. Compiling against earlier versions will result in a warning, but can be dismissed if the system version runtime check for CADisplayLink exists in -awakeFromNib. The runtime check ensures this code will not be called in system versions earlier than 3.1.
            */
            displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawFrame)];
            [displayLink setFrameInterval:animationFrameInterval];
            
            // The run loop will retain the display link on add.
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        }
        else
            animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawFrame) userInfo:nil repeats:TRUE];
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            animationTimer = nil;
        }
        
        animating = FALSE;
    }
}

- (void)drawFrame
{
	lastTime = currentTime;
	currentTime = getCurrentTime();
	btUpdate(currentTime - lastTime);
    [(EAGLView *)self.view setFramebuffer];
    
	btDraw();
    [(EAGLView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return TRUE;
}

- (BOOL)canBecomeFirstResponder {
	return TRUE;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	int i = 0;
	// TODO - track individual touch points and update state appropriately.  This won't work correctly for multitouch
	for (UITouch *touch in touches) {
		CGPoint touchLocation = [touch locationInView: [touch view]];
		btSetPointerState(i++, TRUE, touchLocation.x, touchLocation.y);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	int i = 0;
	for (UITouch *touch in touches) {
		CGPoint touchLocation = [touch locationInView: [touch view]];
		btSetPointerState(i++, TRUE, touchLocation.x, touchLocation.y);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	int i = 0;
	for (UITouch *touch in touches) {
		CGPoint touchLocation = [touch locationInView: [touch view]];
		btSetPointerState(i++, FALSE, touchLocation.x, touchLocation.y);
	}
}

@end

double getCurrentTime()
{
	static mach_timebase_info_data_t sTimebaseInfo;
	uint64_t time = mach_absolute_time();
	uint64_t nanos;
	
	// If this is the first time we've run, get the timebase.
	// We can use denom == 0 to indicate that sTimebaseInfo is
	// uninitialised because it makes no sense to have a zero
	// denominator is a fraction.
	if ( sTimebaseInfo.denom == 0 ) {
		(void) mach_timebase_info(&sTimebaseInfo);
	}
	
	// Do the maths.  We hope that the multiplication doesn't
	// overflow; the price you pay for working in fixed point.
	nanos = time * sTimebaseInfo.numer / sTimebaseInfo.denom;
	return ((double)nanos / 1000000000.0);
}