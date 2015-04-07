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
// Name        : batterytechKeyboardDelegate.h
// Description : iOS keyboard handling support
//============================================================================

#ifndef BATTERYTECHKEYBOARDDELEGATE_H_
#define BATTERYTECHKEYBOARDDELEGATE_H_

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

#import <Foundation/Foundation.h>

@interface batterytechKeyboardDelegate : NSObject <UITextViewDelegate> {
}

@end

#endif /* IOS */
#endif /* BATTERYTECHKEYBOARDDELEGATE_H_ */
