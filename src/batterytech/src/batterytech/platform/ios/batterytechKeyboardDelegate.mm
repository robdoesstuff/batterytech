//
//  batterytechKeyboardDelegate.m
//  imm-pinball-apple
//
//  Created by Apple on 1/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "batterytechKeyboardDelegate.h"
#import "../../batterytech.h"
#import "../platformgeneral.h"

@implementation batterytechKeyboardDelegate

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
	NSLog(@"%@, %d", text, range.length);
	for (int i = 0; i < [text length]; i++) {
		char c = [text characterAtIndex:i];
		if (c == 10) {
			_platform_hide_keyboard();
			break;
		} else {
			btKeyPressed(c);
		}
	}
	if ([text length] == 0) {
		btKeyPressed(8);
	}
	return FALSE;
}

@end
