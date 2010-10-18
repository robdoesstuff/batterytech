//
//  batterytech_iosAppDelegate.h
//  batterytech-ios
//
//  Created by Apple on 10/17/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class batterytech_iosViewController;

@interface batterytech_iosAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    batterytech_iosViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet batterytech_iosViewController *viewController;

@end

