/*
 * BitmapUtil.h
 *
 *  Created on: Oct 21, 2011
 *      Author: rgreen
 */

#ifndef BITMAPUTIL_H_
#define BITMAPUTIL_H_

#include "../primitives.h"

namespace BatteryTech {

	class BitmapUtil {
	public:
		BitmapUtil();
		virtual ~BitmapUtil();
		static void scaleDown(const U8 *srcBuffer, U8 *dstBuffer, S32 components, S32 dstWidth, S32 dstHeight);
		static void roughScale(const U8 *srcBuffer, U8 *dstBuffer, S32 components, S32 srcWidth, S32 srcHeight, S32 dstWidth, S32 dstHeight);
		static void scaleLine(const U8 *Source, U8 *Target, S32 components, S32 SrcWidth, S32 TgtWidth);
	};

}

#endif /* BITMAPUTIL_H_ */
