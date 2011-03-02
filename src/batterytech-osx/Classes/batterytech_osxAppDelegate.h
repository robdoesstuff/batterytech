//
//  batterytech_osxAppDelegate.h
//  batterytech-osx
//
//  Created by Apple on 3/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface batterytech_osxAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
