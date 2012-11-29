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
// Name        : BitmapUtil.cpp
// Description : Some basic bitmap utilities
//============================================================================

#include "BitmapUtil.h"
#include <string.h>

#define SRC(x,y) ( ( (y) * srcWidth + (x)) * components )

namespace BatteryTech {

BitmapUtil::BitmapUtil() {
}

BitmapUtil::~BitmapUtil() {
}

void BitmapUtil::scaleLine(const U8 *Source, U8 *Target, S32 components,
		S32 SrcWidth, S32 TgtWidth) {
	int NumPixels = TgtWidth;
	int IntPart = SrcWidth / TgtWidth;
	int FractPart = SrcWidth % TgtWidth;
	int E = 0;
	int c = 0;

	while (NumPixels-- > 0) {
		for (c = 0; c < components; ++c) {
			*(Target+c) = *(Source+c);
		}
		Target+=components;
		Source += IntPart*components;
		E += FractPart;
		if (E >= TgtWidth) {
			E -= TgtWidth;
			Source+=components;
		} /* if */
	} /* while */
}

void BitmapUtil::roughScale(const U8 *srcBuffer, U8 *dstBuffer, S32 components,
		S32 srcWidth, S32 srcHeight, S32 dstWidth, S32 dstHeight) {
	int NumPixels = dstHeight;
	int IntPart = (srcHeight / dstHeight) * srcWidth;
	int FractPart = srcHeight % dstHeight;
	int E = 0;
	const U8 *PrevSource = NULL;

	while (NumPixels-- > 0) {
		if (srcBuffer == PrevSource) {
			memcpy(dstBuffer, dstBuffer - dstWidth * components,
					dstWidth * sizeof(*dstBuffer) * components);
		} else {
			scaleLine(srcBuffer, dstBuffer, components, srcWidth, dstWidth);
			PrevSource = srcBuffer;
		} /* if */
		dstBuffer += dstWidth*components;
		srcBuffer += IntPart*components;
		E += FractPart;
		if (E >= dstHeight) {
			E -= dstHeight;
			srcBuffer += srcWidth*components;
		}
	}
}

void BitmapUtil::scaleDown(const U8 *srcBuffer, U8 *dstBuffer, S32 components,
		S32 dstWidth, S32 dstHeight) {
	S32 x, y, c;
	S32 srcWidth = dstWidth << 1;

	// Scale in half by a simple box filter (4 samples)
	for (y = 0; y < dstHeight; ++y) {
		for (x = 0; x < dstWidth; ++x) {
			S32 dstI = (y * dstWidth + x) * components;
			for (c = 0; c < components; ++c) {
				dstBuffer[dstI + c] = (srcBuffer[SRC(x<<1,y<<1) + c]
						+ srcBuffer[SRC((x<<1)+1,y<<1) + c]
						+ srcBuffer[SRC(x<<1,(y<<1)+1) + c]
						+ srcBuffer[SRC((x<<1)+1,(y<<1)+1) + c]) >> 2;
			}
		}
	}
}

}
