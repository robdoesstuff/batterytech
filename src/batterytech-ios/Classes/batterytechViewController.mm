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

#import "batterytechViewController.h"
#import "EAGLView.h"
#include "../../batterytech/src/batterytech/batterytech.h"
#include "../../batterytech/src/batterytech/render/GraphicsConfiguration.h"

#define USE_GLES2_WHEN_AVAILABLE FALSE

static GraphicsConfiguration *gConfig;
static double currentTime;
static double lastTime;
double getCurrentTime();
UIView *batterytechRootView;

@interface batterytechViewController ()
@property (nonatomic, retain) EAGLContext *context;
@end

@implementation batterytechViewController

@synthesize context;

- (void)awakeFromNib
{
	batterytechRootView = self.view;
	//touchIds = [[NSMutableArray alloc] initWithCapacity:10];
	touchIds = new UITouch*[MAX_TOUCHES];
	for (int i = 0; i < MAX_TOUCHES; i++) {
		touchIds[i] = 0;
	}
	gConfig = new GraphicsConfiguration;
	EAGLContext *aContext = NULL;
	if (USE_GLES2_WHEN_AVAILABLE) {
		aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	}
	if (!aContext) {
		aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if (aContext) {
			NSLog(@"Initialized GLES 1.0/1.1 Context");
		}
	} else {
		NSLog(@"Initialized GLES 2.0 Context");
		gConfig->supportsShaders = TRUE;
	}
    
    if (!aContext) {
        NSLog(@"Failed to create ES context");
	} else if (![EAGLContext setCurrentContext:aContext]) {
        NSLog(@"Failed to set ES context current");
	}
    
	self.context = aContext;
	[aContext release];
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
	[(EAGLView *)self.view setMultipleTouchEnabled:YES];
	
    animating = FALSE;
    displayLinkSupported = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
    
    // Use of CADisplayLink requires iOS version 3.1 or greater.
	// The NSTimer object is used as fallback when it isn't available.
    NSString *reqSysVer = @"3.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
        displayLinkSupported = TRUE;
	
	gConfig->supportsHWmipmapgen = TRUE;
	gConfig->supportsVBOs = TRUE;
	gConfig->supportsUVTransform = TRUE;
	// TODO - why do we need to reverse these for landscape?? Should be width, height
	int vpWidth = [(EAGLView *)self.view getFBWidth];
	int vpHeight = [(EAGLView *)self.view getFBHeight];
	btInit(gConfig, vpWidth, vpHeight);
	currentTime = getCurrentTime();
	//allocate the audio player∫
	player = [[RemoteIOPlayer alloc]init];
	//initialise the audio player
	[player initialiseAudio];
	[player start];
}

- (void)dealloc
{
    // Tear down context.
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
	}
    [context release];
	[player cleanUp];
	[player release];
	delete [] touchIds;
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
	[player stop];
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
	// store memory address of UITouch using (int)(void*)touch in an array of max 10 size to track touch ids
	for (UITouch *touch in touches) {
		//NSLog(@"New Touch %i", (int)(void*)touch);
		//NSNumber *touchId = [NSNumber numberWithInt:(int)(void*)touch];
		int idx = -1;
		for (int i = 0; i < MAX_TOUCHES; i++) {
			if (touchIds[i] == 0) {
				touchIds[i] = touch;
				idx = i;
				break;
			}
		}
		CGPoint touchLocation = [touch locationInView: [touch view]];
		CGFloat scale = [(EAGLView *)self.view scaleFactor];
		btSetPointerState(idx, TRUE, touchLocation.x * scale, touchLocation.y * scale);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		//NSNumber *touchId = [NSNumber numberWithInt:(int)(void*)touch];
		//NSLog(@"Touch Moved %i", (int)(void*)touch);
		//NSUInteger idx = [touchIds indexOfObject:touch];
		int idx = -1;
		for (int i = 0; i < MAX_TOUCHES; i++) {
			if (touchIds[i] == touch) {
				idx = i;
				break;
			}
		}
		if (idx > -1) {
			CGPoint touchLocation = [touch locationInView: [touch view]];
			CGFloat scale = [(EAGLView *)self.view scaleFactor];
			btSetPointerState(idx, TRUE, touchLocation.x * scale, touchLocation.y * scale);
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		//NSLog(@"Touch Ended %i", (int)(void*)touch);
		//NSNumber *touchId = [NSNumber numberWithInt:(int)(void*)touch];
		int idx = -1;
		for (int i = 0; i < MAX_TOUCHES; i++) {
			if (touchIds[i] == touch) {
				idx = i;
				break;
			}
		}
		if (idx > -1) {
			CGPoint touchLocation = [touch locationInView: [touch view]];
			CGFloat scale = [(EAGLView *)self.view scaleFactor];
			btSetPointerState(idx, FALSE, touchLocation.x * scale, touchLocation.y * scale);
			touchIds[idx] = 0;
		}
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		//NSLog(@"Touch Cancelled %i", (int)(void*)touch);
		//NSNumber *touchId = [NSNumber numberWithInt:(int)(void*)touch];
		int idx = -1;
		for (int i = 0; i < MAX_TOUCHES; i++) {
			if (touchIds[i] == touch) {
				idx = i;
				break;
			}
		}
		if (idx > -1) {
			CGPoint touchLocation = [touch locationInView: [touch view]];
			CGFloat scale = [(EAGLView *)self.view scaleFactor];	
			btSetPointerState(idx, FALSE, touchLocation.x * scale, touchLocation.y * scale);
			touchIds[idx] = 0;		
		}
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
