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
// Name        : Vector4.h
// Description : 4D Vector Structure and Operations
// License     : This source is a derivative work of BSD-licensed source code.  See license below.
//============================================================================

/*
 * vmath, set of classes for computer graphics mathemtics.
 * Copyright (c) 2005-2009, Jan Bartipan < barzto at gmail dot com >
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * - Neither the names of its contributors may be used to endorse or
 *   promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VECTOR4_H_
#define VECTOR4_H_

#include <math.h>
#include <string.h>
#include <assert.h>

#ifndef DEG2RAD
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#define DEG2RAD(x) ((x * M_PI) / 180.0)
   //#define EPSILON (4.37114e-07)

   const double epsilon = 4.37114e-05;
#endif
#ifndef EPSILON
#define EPSILON epsilon
#endif

namespace BatteryTech {

/**
    * Class for four dimensional vector.
    */
   template <class T>
   struct Vector4{
      public:

	 union
	 {
	       /**
		* First element of vector, alias for R-coordinate.
		* For color notation.
		*/
	       T r
	       /**
		* First element of vector, alias for X-coordinate.
		*/;
	       T x;
	 };

	 union
	 {
	       /**
		* Second element of vector, alias for G-coordinate.
		* For color notation.
		*/
	       T g;
	       /**
		* Second element of vector, alias for Y-coordinate.
		*/
	       T y;
	 };

	 union
	 {
	       /**
		* Third element of vector, alias for B-coordinate.
		* For color notation.
		*/
	       T b;
	       /**
		* Third element of vector, alias for Z-coordinate.
		*/
	       T z;
	 };

	 union
	 {
	       /**
		* Fourth element of vector, alias for A-coordinate.
		* For color notation. This represnt aplha chanell
		*/
	       T a;
	       /**
		* First element of vector, alias for W-coordinate.
		* @note For vectors (such as normals) should be set to 0.0
		* For vertices should be set to 1.0
		*/
	       T w;
	 };

	 //----------------[ constructors ]--------------------------
	 /**
	  * Creates and sets to (0,0,0,0)
	  */
	 Vector4() : x(0),y(0),z(0),w(0)
	 { }


	 /**
	  * Creates and sets to (x,y,z,z)
	  * @param nx intial x-coordinate value (R)
	  * @param ny intial y-coordinate value (G)
	  * @param nz intial z-coordinate value (B)
	  * @param nw intial w-coordinate value (Aplha)
	  */
	 Vector4(T nx, T ny, T nz, T nw) : x(nx), y(ny), z(nz), w(nw)
	 { }

	 /**
	  * Copy constructor.
	  * @param src Source of data for new created Vector4 instance.
	  */
	 Vector4(const Vector4<T>& src)
	    : x(src.x), y(src.y), z(src.z), w(src.w)
	 {}

	 /**
	  * Copy casting constructor.
	  * @param src Source of data for new created Vector4 instance.
	  */
	 template <class FromT>
	 Vector4(const Vector4<FromT>& src)
	    : x(static_cast<T>(src.x)),
	      y(static_cast<T>(src.y)),
	      z(static_cast<T>(src.z)),
	      w(static_cast<T>(src.w))
	 {}


	 //----------------[ access operators ]-------------------
	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator=(const Vector4<T>& rhs)
	 {
	    x = rhs.x;
	    y = rhs.y;
	    z = rhs.z;
	    w = rhs.w;
	    return * this;
	 }

	 /**
	  * Copy casting operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 template<class FromT>
	 Vector4<T> operator=(const Vector4<FromT>& rhs)
	 {
	    x = static_cast<T>(rhs.x);
	    y = static_cast<T>(rhs.y);
	    z = static_cast<T>(rhs.z);
	    w = static_cast<T>(rhs.w);
	    return * this;
	 }


	 /**
	  * Array access operator
	  * @param n Array index
	  * @return For n = 0, reference to x coordinate, n = 1
	  * reference to y coordinate, n = 2 reference to z,
	  * else reference to w coordinate.
	  */
	 T & operator[](int n)
	 {
	    assert(n >= 0 && n <= 3);
	    if (0 == n) return x;
	    else if (1 == n) return y;
	    else if (2 == n) return z;
	    else return w;
	 }


	 //---------------[ vector aritmetic operator ]--------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator+(const Vector4<T>& rhs) const
	 {
	    return Vector4<T> (x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator-(const Vector4<T>& rhs) const
	 {
	    return Vector4<T> (x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator*(const Vector4<T> rhs) const
	 {
	    return Vector4<T> (x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator/(const Vector4<T>& rhs) const
	 {
	    return Vector4<T> (x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
	 }

	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator+=(const Vector4<T>& rhs)
	 {
	    x += rhs.x;
	    y += rhs.y;
	    z += rhs.z;
	    w += rhs.w;
	    return * this;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator-=(const Vector4<T>& rhs)
	 {
	    x -= rhs.x;
	    y -= rhs.y;
	    z -= rhs.z;
	    w -= rhs.w;
	    return * this;
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator*=(const Vector4<T>& rhs)
	 {
	    x *= rhs.x;
	    y *= rhs.y;
	    z *= rhs.z;
	    w *= rhs.w;
	    return * this;
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator/=(const Vector4<T>& rhs)
	 {
	    x /= rhs.x;
	    y /= rhs.y;
	    z /= rhs.z;
	    w /= rhs.w;
	    return * this;
	 }

	 //--------------[ equiality operator ]------------------------
	 /**
	  * Equality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @note Test of equality is based of threshold EPSILON value. To be two
	  * values equal, must satisfy this condition | lws.x - rhs.y | < EPSILON,
	  * same for y-coordinate, z-coordinate, and w-coordinate.
	  */
	 bool operator==(const Vector4<T>& rhs) const
	 {
	    return fabs(x - rhs.x) < EPSILON
	       && fabs(y - rhs.y) < EPSILON
	       && fabs(z - rhs.z) < EPSILON
	       && fabs(w - rhs.w) < EPSILON;
	 }


	 /**
	  * Inequality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @return not (lhs == rhs) :-P
	  */
	 bool operator!=(const Vector4<T>& rhs) const { return ! (*this == rhs); }

	 //-------------[ unary operations ]--------------------------
	 /**
	  * Unary negate operator
	  * @return negated vector
	  */
	 Vector4<T> operator-() const
	 {
	    return Vector4<T>(-x, -y, -z, -w);
	 }

	 //--------------[ scalar vector operator ]--------------------

	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator+(T rhs) const
	 {
	    return Vector4<T> (x + rhs, y + rhs, z + rhs, w + rhs);
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator-(T rhs) const
	 {
	    return Vector4<T> (x - rhs, y - rhs, z - rhs, w - rhs);
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator*(T rhs) const
	 {
	    return Vector4<T> (x * rhs, y * rhs, z * rhs, w * rhs);
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator/(T rhs) const
	 {
	    return Vector4<T> (x / rhs, y / rhs, z / rhs, w / rhs);
	 }

	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator+=(T rhs)
	 {
	    x += rhs;
	    y += rhs;
	    z += rhs;
	    w += rhs;
	    return * this;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator-=(T rhs)
	 {
	    x -= rhs;
	    y -= rhs;
	    z -= rhs;
	    w -= rhs;
	    return * this;
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator*=(T rhs)
	 {
	    x *= rhs;
	    y *= rhs;
	    z *= rhs;
	    w *= rhs;
	    return * this;
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T>& operator/=(T rhs)
	 {
	    x /= rhs;
	    y /= rhs;
	    z /= rhs;
	    w /= rhs;
	    return * this;
	 }

	 //-------------[ size operations ]---------------------------
	 /**
	  * Get lenght of vector.
	  * @return lenght of vector
	  */
	 T length() const
	 {
	    return (T)sqrt(x * x + y * y + z * z + w * w);
	 }

	 /**
	  * Normalize vector
	  */
	 void normalize()
	 {
	    T s = length();
	    x /= s;
	    y /= s;
	    z /= s;
	    w /= s;
	 }

	 /**
	  * Return square of length.
	  * @return lenght ^ 2
	  * @note This method is faster then length(). For comparsion
	  * of length of two vector can be used just this value, instead
	  * of computionaly more expensive length() method.
	  */
	 T lengthSq() const
	 {
	    return x * x + y * y + z * z + w * w;
	 }

	 //--------------[ misc. operations ]-----------------------
	 /**
	  * Linear interpolation of two vectors
	  * @param fact Factor of interpolation. For translation from positon
	  * of this vector to vector r, values of factor goes from 0.0 to 1.0.
	  * @param r Second Vector for interpolation
	  * @note Hovewer values of fact parameter are reasonable only in interval
	  * [0.0 , 1.0], you can pass also values outside of this interval and you
	  * can get result (extrapolation?)
	  */
	 Vector4<T> lerp(T fact, const Vector4<T>& r) const
	 {
	    return (*this) + (r - (*this)) * fact;
	 }




	 //-------------[ conversion ]-----------------------------

	 /**
	  * Conversion to pointer operator
	  * @return Pointer to internaly stored (in managment of class Vector4<T>)
	  * used for passing Vector4<T> values to gl*4[fd] functions.
	  */
	 operator T*(){ return (T*) this; }


	 /**
	  * Conversion to pointer operator
	  * @return Constant Pointer to internaly stored (in managment of class Vector4<T>)
	  * used for passing Vector4<T> values to gl*4[fd] functions.
	  */
	 operator const T*() const { return (const T*) this; }

	 //-------------[ output operator ]------------------------
	 /**
	  * Output to stream operator
	  * @param lhs Left hand side argument of operator (commonly ostream instance).
	  * @param rhs Right hand side argument of operator.
	  * @return Left hand side argument - the ostream object passed to operator.
	  */
	 /*
	 friend ostream& operator<<(ostream& lhs, const Vector4<T>& rhs)
	 {
	    lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z << "," << rhs.w << "]";
	    return lhs;
	 }*/
   };

   /// Four dimensional Vector of floats
   typedef Vector4<float> Vector4f;
   /// Four dimensional Vector of doubles
   typedef Vector4<double> Vector4d;
   /// Four dimensional Vector of integers
   typedef Vector4<int> Vector4i;

   typedef Vector4<unsigned char> Vector4b;
}

#endif /* VECTOR4_H_ */
