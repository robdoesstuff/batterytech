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
// Name        : BitmapUtil.h
// Description : Some basic bitmap utilities
//============================================================================

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
