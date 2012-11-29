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
// Name        : ByteUtil.h
// Description : Utility to encode/decode from multi-byte values to byte arrays.
//============================================================================

#ifndef BYTEUTIL_H_
#define BYTEUTIL_H_

#include <batterytech/primitives.h>

namespace BatteryTech {

	class ByteUtil {
	public:
		union Union64 {
			U64 u;
			S64 s;
			F64 f;
			char c[8];
		};
		union Union32 {
			U32 u;
			S32 s;
			F32 f;
			BOOL32 b;
			char c[4];
		};
		union Union16 {
			U16 u;
			S16 s;
			char c[2];
		};
		virtual ~ByteUtil(){};
		/** Puts the 64 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, F64 value);
		/** Puts the 64 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, S64 value);
		/** Puts the 64 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, U64 value);
		/** Puts the 32 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, F32 value);
		/** Puts the 32 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, S32 value);
		/** Puts the 32 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, U32 value);
		/** Puts the 16 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, U16 value);
		/** Puts the 16 bit value into the byte array in little endian order */
		static void toBytes(char *buf, S32 offset, S16 value);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static F64 fromBytesF64(const char *buf, S32 offset);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static S64 fromBytesS64(const char *buf, S32 offset);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static U64 fromBytesU64(const char *buf, S32 offset);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static F32 fromBytesF32(const char *buf, S32 offset);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static S32 fromBytesS32(const char *buf, S32 offset);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static U32 fromBytesU32(const char *buf, S32 offset);
		/** Gets a 32 bit value from the byte array in little endian and converts back to host endian */
		static BOOL32 fromBytesBOOL32(const char *buf, S32 offset);
		/** Gets a 16 bit value from the byte array in little endian and converts back to host endian */
		static U16 fromBytesU16(const char *buf, S32 offset);
		/** Gets a 16 bit value from the byte array in little endian and converts back to host endian */
		static S16 fromBytesS16(const char *buf, S32 offset);
	};

}
#endif /* BYTEUTIL_H_ */
