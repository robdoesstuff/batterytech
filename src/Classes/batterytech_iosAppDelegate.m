//
//  batterytech_iosAppDelegate.m
//  batterytech-ios
//
//  Created by Apple on 10/17/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "batterytech_iosAppDelegate.h"
#import "batterytech_iosViewController.h"

@implementation batterytech_iosAppDelegate

@synthesize window;
@synthesize viewController;

- (void)applicationWillResignActive:(UIApplication *)application
{
    [viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [viewController stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end
