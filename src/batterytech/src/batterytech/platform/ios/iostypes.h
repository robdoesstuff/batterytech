/*
 *  iostypes.h
 *  batterytech-ios
 *
 *  Created by Apple on 10/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IOSTYPES_H_
#define IOSTYPES_H_

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE

// integers
typedef signed char S8;
typedef unsigned char U8;
typedef signed short S16;
typedef unsigned short U16;
typedef signed int S32;
typedef unsigned int U32;
typedef signed long long S64;
typedef unsigned long long U64;

// floats (real numbers)
typedef float F32;
typedef double F64;

// byte and boolean
typedef unsigned char BYTE8;
typedef U32 BOOL32;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* IOS */
#endif /* IOSTYPES_H_ */