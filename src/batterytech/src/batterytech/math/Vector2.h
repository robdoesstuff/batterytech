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
// Name        : Vector2.h
// Description : 2D Vector Structure and Operations
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

#ifndef VECTOR2_H_
#define VECTOR2_H_

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
* Class for two dimensional vector.
*/
template <class T>
struct Vector2
{
  public:
 union {
	   /**
	* First element of vector, alias for X-coordinate.
	*/
	   T x;

	   /**
	* First element of vector, alias for S-coordinate.
	* For textures notation.
	*/
	   T s;
 } ;

 union {
	   /**
	* Second element of vector, alias for Y-coordinate.
	*/
	   T y;

	   /**
	* Second element of vector, alias for T-coordinate.
	* For textures notation.
	*/
	   T t;
 } ;

 //----------------[ constructors ]--------------------------
 /**
  * Creates and sets to (0,0)
  */
 Vector2() : x(0),y(0)
 {	}



 /**
  * Creates and sets to (x,y)
  * @param nx intial x-coordinate value
  * @param ny intial y-coordinate value
  */
 Vector2(T nx, T ny) : x(nx), y(ny)
 {	}


 /**
  * Copy constructor.
  * @param src Source of data for new created instace.
  */
 Vector2(const Vector2<T>& src)
	: x(src.x), y(src.y)
 {	 }


 /**
  * Copy casting constructor.
  * @param src Source of data for new created instace.
  */
 template <class FromT>
 Vector2(const Vector2<FromT>& src)
	: x(static_cast<T>(src.x)),
	  y(static_cast<T>(src.y))
 {	 }


 //----------------[ access operators ]-------------------
 /**
  * Copy casting operator
  * @param rhs Right hand side argument of binary operator.
  */
 template <class FromT>
 Vector2<T>& operator=(const Vector2<FromT>& rhs)
 {
	x = static_cast<T>(rhs.x);
	y = static_cast<T>(rhs.y);
	return * this;
 }

 /**
  * Copy operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator=(const Vector2<T>& rhs)
 {
	x = rhs.x;
	y = rhs.y;
	return * this;
 }


 /**
  * Array access operator
  * @param n Array index
  * @return For n = 0, reference to x coordinate, else reference to y
  * y coordinate.
  */
 T& operator[](int n)
 {
	assert(n >= 0 && n <= 1);
	if (0 == n) return x;
	else
	   return y;
 }


 //---------------[ vector aritmetic operator ]--------------
 /**
  * Addition operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator+(const Vector2<T>& rhs) const
 {
	return Vector2<T> (x + rhs.x, y + rhs.y);
 }

 /**
  * Substraction operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator-(const Vector2<T>& rhs) const
 {
	return Vector2<T> (x - rhs.x, y - rhs.y);
 }

 /**
  * Multiplication operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator*(const Vector2<T>& rhs) const
 {
	return Vector2<T> (x * rhs.x, y * rhs.y);
 }

 /**
  * Division operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator/(const Vector2<T>& rhs) const
 {
	return Vector2<T> (x / rhs.x, y / rhs.y);
 }

 /**
  * Addition operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator+=(const Vector2<T>& rhs)
 {
	x += rhs.x;
	y += rhs.y;
	return * this;
 }

 /**
  * Substraction operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator-=(const Vector2<T>& rhs)
 {
	x -= rhs.x;
	y -= rhs.y;
	return * this;
 }

 /**
  * Multiplication operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator*=(const Vector2<T>& rhs)
 {
	x *= rhs.x;
	y *= rhs.y;
	return * this;
 }

 /**
  * Division operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator/=(const Vector2<T>& rhs)
 {
	x /= rhs.x;
	y /= rhs.y;
	return * this;
 }

 //--------------[ scalar vector operator ]--------------------
 /**
  * Addition operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator+(T rhs) const
 {
	return Vector2<T> (x + rhs, y + rhs);
 }

 /**
  * Substraction operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator-(T rhs) const
 {
	return Vector2<T> (x - rhs, y - rhs);
 }

 /**
  * Multiplication operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator*(T rhs) const
 {
	return Vector2<T> (x * rhs, y * rhs);
 }

 /**
  * Division operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T> operator/(T rhs) const
 {
	return Vector2<T> (x / rhs, y / rhs);
 }

 /**
  * Addition operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator+=(T rhs)
 {
	x += rhs;
	y += rhs;
	return * this;
 }

 /**
  * Substraction operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator-=(T rhs)
 {
	x -= rhs;
	y -= rhs;
	return * this;
 }

 /**
  * Multiplication operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator*=(T rhs)
 {
	x *= rhs;
	y *= rhs;
	return * this;
 }

 /**
  * Division operator
  * @param rhs Right hand side argument of binary operator.
  */
 Vector2<T>& operator/=(T rhs)
 {
	x /= rhs;
	y /= rhs;
	return * this;
 }

 //--------------[ equality operator ]------------------------
 /**
  * Equality test operator
  * @param rhs Right hand side argument of binary operator.
  * @note Test of equality is based of threshold EPSILON value. To be two
  * values equal, must satisfy this condition | lws.x - rhs.y | < EPSILON,
  * same for y-coordinate.
  */
 bool operator==(const Vector2<T>& rhs) const
 {
	return (abs(x - rhs.x) < EPSILON) && (abs(y - rhs.y) < EPSILON);
 }


 /**
  * Inequality test operator
  * @param rhs Right hand side argument of binary operator.
  * @return not (lhs == rhs) :-P
  */
 bool operator!=(const Vector2<T>& rhs) const { return ! (*this == rhs); }

 //-------------[ unary operations ]--------------------------
 /**
  * Unary negate operator
  * @return negated vector
  */
 Vector2<T> operator-() const
 {
	return Vector2<T>(-x, -y);
 }

 //-------------[ size operations ]---------------------------
 /**
  * Get lenght of vector.
  * @return lenght of vector
  */
 T length() const
 {
	return (T)sqrt(x * x + y * y);
 }

 /**
  * Normalize vector
  */
 void normalize()
 {
	T s = length();
	x /= s;
	y /= s;
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
	return x * x + y * y;
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
 Vector2<T> lerp(T fact, const Vector2<T>& r) const
 {
	return (*this) + (r - (*this)) * fact;
 }


 //-------------[ conversion ]-----------------------------
 /**
  * Conversion to pointer operator
  * @return Pointer to internaly stored (in managment of class Vector2<T>)
  * used for passing Vector2<T> values to gl*2[fd] functions.
  */
 operator T*(){ return (T*) this; }
 /**
  * Conversion to pointer operator
  * @return Constant Pointer to internaly stored (in managment of class Vector2<T>)
  * used for passing Vector2<T> values to gl*2[fd] functions.
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
 friend ostream& operator<<(ostream& lhs, const Vector2<T>& rhs)
 {
	lhs << "[" << rhs.x << "," << rhs.y << "]";
	return lhs;
 }*/


};


//--------------------------------------
// Typedef shortcuts for 2D vector
//-------------------------------------
/// Two dimensional Vector of floats
typedef Vector2 <float> Vector2f;
/// Two dimensional Vector of doubles
typedef Vector2 <double> Vector2d;
/// Two dimension Vector of ints
typedef Vector2 <int> Vector2i;
}

#endif /* VECTOR2_H_ */
