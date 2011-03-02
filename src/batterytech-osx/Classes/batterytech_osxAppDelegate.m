//
//  batterytech_osxAppDelegate.m
//  batterytech-osx
//
//  Created by Apple on 3/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "batterytech_osxAppDelegate.h"

@implementation batterytech_osxAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

@end
