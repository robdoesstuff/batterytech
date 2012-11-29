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
// Name        : blackberrytypes.h
// Description : blackberry primitive definitions
// Usage       : Use primitives.h.  Do not include this directly.
//============================================================================

#ifndef BLACKBERRYTYPES_H_
#define BLACKBERRYTYPES_H_
#if defined(__QNXNTO__)

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

#endif /* BLACKBERRY */
#endif /* BLACKBERRYTYPES_H_ */
