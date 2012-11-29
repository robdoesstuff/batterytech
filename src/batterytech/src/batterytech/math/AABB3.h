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
// Name        : AABB3.h
// Description : 3D Axis Aligned Bounding Box
//============================================================================

#ifndef AABB3_H_
#define AABB3_H_

#include "Vector3.h"

namespace BatteryTech {

	template <class T>
	struct AABB3 {
		Vector3<T> pMin;
		Vector3<T> pMax;

		//----------------[ constructors ]--------------------------
		AABB3() {	}

		AABB3(const Vector3<T> &nMin, const Vector3<T> &nMax) : pMin(nMin), pMax(nMax)
			 {	}

			 /**
			  * Copy constructor.
			  * @param src Source of data for new created AABB3 instance.
			  */
		AABB3(const AABB3<T>& src)
			    : pMin(src.pMin), pMax(src.pMax)
			 {}

		 /**
		  * Copy casting constructor.
		  * @param src Source of data for new created AABB3 instance.
		  */
		 template <class FromT>
		 AABB3(const AABB3<FromT>& src)
			: pMin(static_cast<Vector3<T> >(src.pMin)),
			  pMax(static_cast<Vector3<T> >(src.pMax))
		 {}

		 //----------------[ access operators ]-------------------
		 /**
		  * Copy operator
		  * @param rhs Right hand side argument of binary operator.
		  */
		 AABB3<T> operator=(const AABB3<T>& rhs)
		 {
			pMin = rhs.pMin;
			pMax = rhs.pMax;
			return * this;
		 }

		 /**
		  * Copy casting operator.
		  * @param rhs Right hand side argument of binary operator.
		  */
		 template <class FromT>
		 AABB3<T> operator=(const AABB3<FromT>& rhs)
		 {
			pMin(static_cast<Vector3<T> >(rhs.pMin));
			pMax(static_cast<Vector3<T> >(rhs.pMax));
			return * this;
		 }

		 void includePoint(const float x, const float y, const float z) {
			if (x < pMin.x) {
				pMin.x = x;
			}
			if (y < pMin.y) {
				pMin.y = y;
			}
			if (z < pMin.z) {
				pMin.z = z;
			}
			if (x > pMax.x) {
				pMax.x = x;
			}
			if (y > pMax.y) {
				pMax.y = y;
			}
			if (z > pMax.z) {
				pMax.z = z;
			}
		 }

		 void includePoint(const Vector3<T>& rhs) {
			if (rhs.x < pMin.x) {
				pMin.x = rhs.x;
			}
			if (rhs.y < pMin.y) {
				pMin.y = rhs.y;
			}
			if (rhs.z < pMin.z) {
				pMin.z = rhs.z;
			}
			if (rhs.x > pMax.x) {
				pMax.x = rhs.x;
			}
			if (rhs.y > pMax.y) {
				pMax.y = rhs.y;
			}
			if (rhs.z > pMax.z) {
				pMax.z = rhs.z;
			}
		 }

		 void reCalc() {
			if (pMin.x > pMax.x) {
				T temp = pMin.x;
				pMin.x = pMax.x;
				pMax.x = temp;
			}
			if (pMin.y > pMax.y) {
				T temp = pMin.y;
				pMin.y = pMax.y;
				pMax.y = temp;
			}
			if (pMin.z > pMax.z) {
				T temp = pMin.z;
				pMin.z = pMax.z;
				pMax.z = temp;
			}
		 }
	};

   typedef AABB3<float> AABB3f;
   typedef AABB3<double> AABB3d;
   typedef AABB3<int> AABB3i;
}

#endif
