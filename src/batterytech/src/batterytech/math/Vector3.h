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
// Name        : Vector3.h
// Description : 3D Vector Structure and Operations
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

#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <math.h>
#include <string.h>
#include <assert.h>
#include "Vector4.h"

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
    * Class for three dimensional vector.
    */
   template <class T>
   struct Vector3
   {
      public:
	 //T x, y, z;
	 union
	 {
	       /**
		* First element of vector, alias for X-coordinate.
		*/
	       T x;

	       /**
		* First element of vector, alias for S-coordinate.
		* For textures notation.
		*/
	       T s;

	       /**
		* First element of vector, alias for R-coordinate.
		* For color notation.
		*/
	       T r;
	 };

	 union
	 {
	       /**
		* Second element of vector, alias for Y-coordinate.
		*/
	       T y;
	       /**
		* Second element of vector, alias for T-coordinate.
		* For textures notation.
		*/
	       T t;
	       /**
		* Second element of vector, alias for G-coordinate.
		* For color notation.
		*/
	       T g;
	 };

	 union
	 {
	       /**
		* Third element of vector, alias for Z-coordinate.
		*/
	       T z;

	       /**
		* Third element of vector, alias for U-coordinate.
		* For textures notation.
		*/
	       T u;
	       /**
		* Third element of vector, alias for B-coordinate.
		* For color notation.
		*/
	       T b;
	 };

	 //----------------[ constructors ]--------------------------
	 /**
	  * Creates and sets to (0,0,0)
	  */
	 Vector3() : x(0),y(0),z(0)
	 {	}

	 /**
	  * Creates and sets to (x,y,z)
	  * @param nx intial x-coordinate value
	  * @param ny intial y-coordinate value
	  * @param nz intial z-coordinate value
	  */
	 Vector3(T nx, T ny, T nz) : x(nx),y(ny),z(nz)
	 {	}

	 /**
	  * Copy constructor.
	  * @param src Source of data for new created Vector3 instance.
	  */
	 Vector3(const Vector3<T>& src)
	    : x(src.x), y(src.y), z(src.z)
	 {}

	 /**
	  * Copy casting constructor.
	  * @param src Source of data for new created Vector3 instance.
	  */
	 template <class FromT>
	 Vector3(const Vector3<FromT>& src)
	    : x(static_cast<T>(src.x)),
	      y(static_cast<T>(src.y)),
	      z(static_cast<T>(src.z))
	 {}


	 //----------------[ access operators ]-------------------
	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator=(const Vector3<T>& rhs)
	 {
	    x = rhs.x;
	    y = rhs.y;
	    z = rhs.z;
	    return * this;
	 }

	 /**
	  * Copy casting operator.
	  * @param rhs Right hand side argument of binary operator.
	  */
	 template <class FromT>
	 Vector3<T> operator=(const Vector3<FromT>& rhs)
	 {
	    x = static_cast<T>(rhs.x);
	    y = static_cast<T>(rhs.y);
	    z = static_cast<T>(rhs.z);
	    return * this;
	 }

	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator=(const Vector4<T>& rhs)
	 {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return * this;
	 }

	 /**
	  * Array access operator
	  * @param n Array index
	  * @return For n = 0, reference to x coordinate, n = 1
	  * reference to y, else reference to z
	  * y coordinate.
	  */
	 T & operator[](int n)
	 {
	    assert(n >= 0 && n <= 2);
	    if (0 == n) return x;
	    else if (1 == n) return y;
	    else
	       return z;
	 }


	 //---------------[ vector arithmetic operator ]--------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator+(const Vector3<T>& rhs) const
	 {
	    return Vector3<T> (x + rhs.x, y + rhs.y, z + rhs.z);
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator-(const Vector3<T>& rhs) const
	 {
	    return Vector3<T> (x - rhs.x, y - rhs.y, z - rhs.z);
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator*(const Vector3<T>& rhs) const
	 {
	    return Vector3<T> (x * rhs.x, y * rhs.y, z * rhs.z);
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator/(const Vector3<T>& rhs) const
	 {
	    return Vector3<T> (x / rhs.x, y / rhs.y, z / rhs.z);
	 }

	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator+=(const Vector3<T>& rhs)
	 {
	    x += rhs.x;
	    y += rhs.y;
	    z += rhs.z;
	    return * this;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator-=(const Vector3<T>& rhs)
	 {
	    x -= rhs.x;
	    y -= rhs.y;
	    z -= rhs.z;
	    return * this;
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator*=(const Vector3<T>& rhs)
	 {
	    x *= rhs.x;
	    y *= rhs.y;
	    z *= rhs.z;
	    return * this;
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator/=(const Vector3<T>& rhs)
	 {
	    x /= rhs.x;
	    y /= rhs.y;
	    z /= rhs.z;
	    return * this;
	 }

	 /**
	  * Dot product of two vectors.
	  * @param rhs Right hand side argument of binary operator.
	  */
	 T dot(const Vector3<T>& rhs) const
	 {
	    return x * rhs.x + y * rhs.y + z * rhs.z;
	 }

	 /**
	  * Cross product opertor
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> cross(const Vector3<T>& rhs) const
	 {
	    return Vector3<T> (y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y);
	 }


	 //--------------[ scalar vector operator ]--------------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator+(T rhs) const
	 {
	    return Vector3<T> (x + rhs, y + rhs, z + rhs);
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator-(T rhs) const
	 {
	    return Vector3<T> (x - rhs, y - rhs, z - rhs);
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator*(T rhs) const
	 {
	    return Vector3<T> (x * rhs, y * rhs, z * rhs);
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator/(T rhs) const
	 {
	    return Vector3<T> (x / rhs, y / rhs, z / rhs);
	 }

	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator+=(T rhs)
	 {
	    x += rhs;
	    y += rhs;
	    z += rhs;
	    return * this;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator-=(T rhs)
	 {
	    x -= rhs;
	    y -= rhs;
	    z -= rhs;
	    return * this;
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator*=(T rhs)
	 {
	    x *= rhs;
	    y *= rhs;
	    z *= rhs;
	    return * this;
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T>& operator/=(T rhs)
	 {
	    x /= rhs;
	    y /= rhs;
	    z /= rhs;
	    return * this;
	 }

	 //--------------[ equiality operator ]------------------------
	 /**
	  * Equality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @note Test of equality is based of threshold EPSILON value. To be two
	  * values equal, must satisfy this condition | lws.x - rhs.y | < EPSILON,
	  * same for y-coordinate, and z-coordinate.
	  */
	 bool operator==(const Vector3<T>& rhs) const
	 {
	    return fabs(x - rhs.x) < EPSILON
	       && fabs(y - rhs.y) < EPSILON
	       && fabs(z - rhs.z) < EPSILON;
	 }

	 /**
	  * Inequality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @return not (lhs == rhs) :-P
	  */
	 bool operator!=(const Vector3<T>& rhs) const { return !(*this == rhs); }

	 //-------------[ unary operations ]--------------------------
	 /**
	  * Unary negate operator
	  * @return negated vector
	  */
	 Vector3<T> operator-() const
	 {
	    return Vector3<T>(-x, -y, -z);
	 }

	 //-------------[ size operations ]---------------------------
	 /**
	  * Get lenght of vector.
	  * @return lenght of vector
	  */
	 T length() const
	 {
	    return (T)sqrt(x * x + y * y + z * z);
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
	    return x * x + y * y + z * z;
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
	 }

	 //------------[ other operations ]---------------------------
	 /**
	  * Rotate vector around three axis.
	  * @param ax Angle (in degrees) to be rotated around X-axis.
	  * @param ay Angle (in degrees) to be rotated around Y-axis.
	  * @param az Angle (in degrees) to be rotated around Z-axis.
	  */
	 void rotate(T ax, T ay, T az)
	 {
	    T a = cos(DEG2RAD(ax));
	    T b = sin(DEG2RAD(ax));
	    T c = cos(DEG2RAD(ay));
	    T d = sin(DEG2RAD(ay));
	    T e = cos(DEG2RAD(az));
	    T f = sin(DEG2RAD(az));
	    T nx = c * e * x - c * f * y + d * z;
	    T ny = (a * f + b * d * e) * x + (a * e - b * d * f) * y - b * c * z;
	    T nz = (b * f - a * d * e) * x + (a * d * f + b * e) * y + a * c * z;
	    x = nx; y = ny; z = nz;
	 }

	 /**
	  * Linear interpolation of two vectors
	  * @param fact Factor of interpolation. For translation from positon
	  * of this vector to vector r, values of factor goes from 0.0 to 1.0.
	  * @param r Second Vector for interpolation
	  * @note Hovewer values of fact parameter are reasonable only in interval
	  * [0.0 , 1.0], you can pass also values outside of this interval and you
	  * can get result (extrapolation?)
	  */
	 Vector3<T> lerp(T fact, const Vector3<T>& r) const
	 {
	    return (*this) + (r - (*this)) * fact;
	 }





	 //-------------[ conversion ]-----------------------------

	 /**
	  * Conversion to pointer operator
	  * @return Pointer to internaly stored (in managment of class Vector3<T>)
	  * used for passing Vector3<T> values to gl*3[fd] functions.
	  */
	 operator T*(){ return (T*) this; }

	 /**
	  * Conversion to pointer operator
	  * @return Constant Pointer to internaly stored (in managment of class Vector3<T>)
	  * used for passing Vector3<T> values to gl*3[fd] functions.
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
	 friend ostream& operator<<(ostream& lhs, const Vector3<T> rhs)
	 {
	    lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z  << "]";
	    return lhs;
	 }*/

   };


   /// Three dimensional Vector of floats
   typedef Vector3 <float> Vector3f;
   /// Three dimensional Vector of doubles
   typedef Vector3 <double> Vector3d;
   /// Three dimensional Vector of ints
   typedef Vector3 <int> Vector3i;

}

#endif /* VECTOR3_H_ */
