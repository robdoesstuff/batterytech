//
//  batterytech_iosAppDelegate.h
//  batterytech-ios
//
//  Created by Apple on 10/17/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class batterytechViewController;

@interface batterytechAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    batterytechViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet batterytechViewController *viewController;

@end

