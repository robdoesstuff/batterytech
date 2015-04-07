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
// Name        : batterytechKeyboardDelegate.mm
// Description : iOS keyboard handling support
//============================================================================

#import "batterytechKeyboardDelegate.h"
#import "../../batterytech.h"
#import "../platformgeneral.h"

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE

@implementation batterytechKeyboardDelegate

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
	NSLog(@"%@, %d", text, range.length);
	for (int i = 0; i < [text length]; i++) {
		char c = [text characterAtIndex:i];
		if (c == 10) {
			_platform_hide_keyboard();
			break;
		} else {
			btKeyPressed(c, BatteryTech::SKEY_NULL);
		}
	}
	if ([text length] == 0) {
		btKeyPressed(8, BatteryTech::SKEY_NULL);
	}
	return FALSE;
}

@end

#endif /* IOS */
