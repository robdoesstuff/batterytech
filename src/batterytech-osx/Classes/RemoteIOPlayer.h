//
//  RemoteIOPlayer.h
//  RemoteIOTest
//
//  Created by Aran Mulholland on 3/03/09.
//  Copyright 2009 Aran Mulholland. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RemoteIOPlayer : NSObject {
}

-(OSStatus)start;
-(OSStatus)stop;
-(void)cleanUp;
-(void)initialiseAudio;
-(UInt32)getNextPacket;

@end
