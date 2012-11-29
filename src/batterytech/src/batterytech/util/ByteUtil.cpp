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
// Name        : ByteUtil.cpp
// Description : Utility to encode/decode from multi-byte values to byte arrays.
//============================================================================

#include "ByteUtil.h"

#include <batterytech/platform/platformgeneral.h>

namespace BatteryTech {

	void ByteUtil::toBytes(char *buf, S32 offset, U64 value) {
		// puts bytes in little-endian byte order 0xAABBCCDDEEFF1122 to bytes 7,6,5,4,3,2,1,0
		Union64 un;
		un.u = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
			buf[offset+2] = un.c[2];
			buf[offset+3] = un.c[3];
			buf[offset+4] = un.c[4];
			buf[offset+5] = un.c[5];
			buf[offset+6] = un.c[6];
			buf[offset+7] =  un.c[7];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[7];
			buf[offset+1] = un.c[6];
			buf[offset+2] = un.c[5];
			buf[offset+3] = un.c[4];
			buf[offset+4] = un.c[3];
			buf[offset+5] = un.c[2];
			buf[offset+6] = un.c[1];
			buf[offset+7] =  un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, S64 value) {
		// puts bytes in little-endian byte order 0xAABBCCDDEEFF1122 to bytes 7,6,5,4,3,2,1,0
		Union64 un;
		un.s = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
			buf[offset+2] = un.c[2];
			buf[offset+3] = un.c[3];
			buf[offset+4] = un.c[4];
			buf[offset+5] = un.c[5];
			buf[offset+6] = un.c[6];
			buf[offset+7] =  un.c[7];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[7];
			buf[offset+1] = un.c[6];
			buf[offset+2] = un.c[5];
			buf[offset+3] = un.c[4];
			buf[offset+4] = un.c[3];
			buf[offset+5] = un.c[2];
			buf[offset+6] = un.c[1];
			buf[offset+7] =  un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, F64 value) {
		// puts bytes in little-endian byte order 0xAABBCCDDEEFF1122 to bytes 7,6,5,4,3,2,1,0
		Union64 un;
		un.f = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
			buf[offset+2] = un.c[2];
			buf[offset+3] = un.c[3];
			buf[offset+4] = un.c[4];
			buf[offset+5] = un.c[5];
			buf[offset+6] = un.c[6];
			buf[offset+7] =  un.c[7];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[7];
			buf[offset+1] = un.c[6];
			buf[offset+2] = un.c[5];
			buf[offset+3] = un.c[4];
			buf[offset+4] = un.c[3];
			buf[offset+5] = un.c[2];
			buf[offset+6] = un.c[1];
			buf[offset+7] =  un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, U32 value) {
		// puts bytes in little-endian byte order 0xAABBCCDD to bytes 3,2,1,0
		Union32 un;
		un.u = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
			buf[offset+2] = un.c[2];
			buf[offset+3] =  un.c[3];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[3];
			buf[offset+1] = un.c[2];
			buf[offset+2] = un.c[1];
			buf[offset+3] =  un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, S32 value) {
		// puts bytes in little-endian byte order 0xAABBCCDD to bytes 3,2,1,0
		Union32 un;
		un.s = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
			buf[offset+2] = un.c[2];
			buf[offset+3] =  un.c[3];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[3];
			buf[offset+1] = un.c[2];
			buf[offset+2] = un.c[1];
			buf[offset+3] =  un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, F32 value) {
		// puts bytes in little-endian byte order 0xAABBCCDD to bytes 3,2,1,0
		Union32 un;
		un.f = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
			buf[offset+2] = un.c[2];
			buf[offset+3] =  un.c[3];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[3];
			buf[offset+1] = un.c[2];
			buf[offset+2] = un.c[1];
			buf[offset+3] =  un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, U16 value) {
		// puts bytes in little-endian byte order 0xAABB to bytes 1,0
		Union16 un;
		un.u = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[1];
			buf[offset+1] = un.c[0];
		}
	}

	void ByteUtil::toBytes(char *buf, S32 offset, S16 value) {
		// puts bytes in little-endian byte order 0xAABB to bytes 1,0
		Union16 un;
		un.s = value;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			buf[offset] = un.c[0];
			buf[offset+1] = un.c[1];
		} else {
			// swap bytes from big endian to little
			buf[offset] = un.c[1];
			buf[offset+1] = un.c[0];
		}
	}

	S64 ByteUtil::fromBytesS64(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union64 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
			un.c[4] = *(buf + offset + 4);
			un.c[5] = *(buf + offset + 5);
			un.c[6] = *(buf + offset + 6);
			un.c[7] = *(buf + offset + 7);
		} else {
			un.c[0] = *(buf + offset + 7);
			un.c[1] = *(buf + offset + 6);
			un.c[2] = *(buf + offset + 5);
			un.c[3] = *(buf + offset + 4);
			un.c[4] = *(buf + offset + 3);
			un.c[5] = *(buf + offset + 2);
			un.c[6] = *(buf + offset + 1);
			un.c[7] = *(buf + offset );
		}
		return un.s;
	}

	F64 ByteUtil::fromBytesF64(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union64 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
			un.c[4] = *(buf + offset + 4);
			un.c[5] = *(buf + offset + 5);
			un.c[6] = *(buf + offset + 6);
			un.c[7] = *(buf + offset + 7);
		} else {
			un.c[0] = *(buf + offset + 7);
			un.c[1] = *(buf + offset + 6);
			un.c[2] = *(buf + offset + 5);
			un.c[3] = *(buf + offset + 4);
			un.c[4] = *(buf + offset + 3);
			un.c[5] = *(buf + offset + 2);
			un.c[6] = *(buf + offset + 1);
			un.c[7] = *(buf + offset );
		}
		return un.f;
	}

	U64 ByteUtil::fromBytesU64(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union64 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
			un.c[4] = *(buf + offset + 4);
			un.c[5] = *(buf + offset + 5);
			un.c[6] = *(buf + offset + 6);
			un.c[7] = *(buf + offset + 7);
		} else {
			un.c[0] = *(buf + offset + 7);
			un.c[1] = *(buf + offset + 6);
			un.c[2] = *(buf + offset + 5);
			un.c[3] = *(buf + offset + 4);
			un.c[4] = *(buf + offset + 3);
			un.c[5] = *(buf + offset + 2);
			un.c[6] = *(buf + offset + 1);
			un.c[7] = *(buf + offset );
		}
		return un.u;
	}

	S32 ByteUtil::fromBytesS32(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union32 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
		} else {
			un.c[0] = *(buf + offset + 3);
			un.c[1] = *(buf + offset + 2);
			un.c[2] = *(buf + offset + 1);
			un.c[3] = *(buf + offset);
		}
		return un.s;
	}

	F32 ByteUtil::fromBytesF32(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union32 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
		} else {
			un.c[0] = *(buf + offset + 3);
			un.c[1] = *(buf + offset + 2);
			un.c[2] = *(buf + offset + 1);
			un.c[3] = *(buf + offset);
		}
		return un.f;
	}

	U32 ByteUtil::fromBytesU32(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union32 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
		} else {
			un.c[0] = *(buf + offset + 3);
			un.c[1] = *(buf + offset + 2);
			un.c[2] = *(buf + offset + 1);
			un.c[3] = *(buf + offset);
		}
		return un.u;
	}

	BOOL32 ByteUtil::fromBytesBOOL32(const char *buf, S32 offset) {
		// reads bytes from little endian back to host endian
		Union32 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
			un.c[2] = *(buf + offset + 2);
			un.c[3] = *(buf + offset + 3);
		} else {
			un.c[0] = *(buf + offset + 3);
			un.c[1] = *(buf + offset + 2);
			un.c[2] = *(buf + offset + 1);
			un.c[3] = *(buf + offset);
		}
		return un.b;
	}

	U16 ByteUtil::fromBytesU16(const char *buf, S32 offset) {
		Union16 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
		} else {
			un.c[0] = *(buf + offset + 1);
			un.c[1] = *(buf + offset);
		}
		return un.u;
	}

	S16 ByteUtil::fromBytesS16(const char *buf, S32 offset) {
		Union16 un;
		if (PLATFORM_IS_LITTLE_ENDIAN) {
			un.c[0] = *(buf + offset);
			un.c[1] = *(buf + offset + 1);
		} else {
			un.c[0] = *(buf + offset + 1);
			un.c[1] = *(buf + offset);
		}
		return un.s;
	}

}
