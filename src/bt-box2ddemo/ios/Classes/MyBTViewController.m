/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games, LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : MyBTViewController.h
// Description : IOS Custom View Controller
//============================================================================


#import "MyBTViewController.h"

@interface MyBTViewController ()

@end

@implementation MyBTViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

// BatteryTech overrides (the reason we created this subclass)

- (void)showAd {
}

- (void)hideAd {
}

- (void)hook:(const char*)hookData withResult:(char*) result withResultLen:(int) resultLen {
}

- (bool) useGLES2 {
    return TRUE;
}

- (bool) forceLandscape {
    return TRUE;
}

@end
