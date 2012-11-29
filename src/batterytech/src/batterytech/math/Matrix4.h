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
// Name        : Matrix4.h
// Description : 4x4 Matrix Structure and Operations
// Usage       : OpenGL-Compatible Column-Major Storage
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

#ifndef MATRIX4_H_
#define MATRIX4_H_

#include <math.h>
#include <string.h>
#include <assert.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Vector4.h"
#include "AABB3.h"

#ifndef DEG2RAD
#define DEG2RAD(x) ((x * M_PI) / 180.0)
   //#define EPSILON (4.37114e-07)

   const double epsilon = 4.37114e-05;
#endif
#ifndef EPSILON
#define EPSILON epsilon
#endif
#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif

namespace BatteryTech {

/**
    * Class for matrix 4x4
    * @note Data stored in this matrix are in column major order.
    */
   template <class T>
   struct Matrix4
   {
      public:
	 /// Data stored in column major order
	 T data[16];

	 //--------------------------[ constructors ]-------------------------------
	 /**
	  *Creates identity matrix
	  */
	 Matrix4()
	 {
	    for (int i = 0; i < 16; i++)
	       data[i] = (i % 5) ? 0 : 1;
	 }

	 /**
	  * Copy matrix values from array (these data must be in column
	  * major order!)
	  */
	 Matrix4(const T * dt)
	 {
	    memcpy(data, dt, sizeof(T) * 16);
	 }

	 /**
	  * Copy matrix values from parameters (these data must be in column
	  * major order!)
	  */
	 Matrix4(const T d0, const T d1, const T d2, const T d3, const T d4, const T d5, const T d6, const T d7, const T d8, const T d9, const T d10, const T d11, const T d12, const T d13, const T d14, const T d15) {
		 data[0] = d0;
		 data[1] = d1;
		 data[2] = d2;
		 data[3] = d3;
		 data[4] = d4;
		 data[5] = d5;
		 data[6] = d6;
		 data[7] = d7;
		 data[8] = d8;
		 data[9] = d9;
		 data[10] = d10;
		 data[11] = d11;
		 data[12] = d12;
		 data[13] = d13;
		 data[14] = d14;
		 data[15] = d15;
	 }

	 /**
	  * Copy constructor.
	  * @param src Data source for new created instance of Matrix4.
	  */
	 Matrix4(const Matrix4<T>& src)
	 {
	    memcpy(data, src.data, sizeof(T) * 16);
	 }

	 /**
	  * Copy casting constructor.
	  * @param src Data source for new created instance of Matrix4.
	  */
	 template <class FromT>
	 Matrix4(const Matrix4<FromT>& src)
	 {
	    for (int i = 0; i < 16; i++)
	    {
	       data[i] = static_cast<T>(src.data[i]);
	    }
	 }

	 /**
	  * Resets matrix to be identity matrix
	  */
	 void identity()
	 {
	    for (int i = 0; i < 16; i++) {
	       data[i] = (i % 5) ? 0 : 1;
	    }
	 }



	 /**
	  * Creates rotation matrix by rotation around axis.
	  * @param a Angle (in radians) of rotation around axis X.
	  * @param b Angle (in radians) of rotation around axis Y.
	  * @param c Angle (in radians) of rotation around axis Z.
	  */
	 static Matrix4<T> createRotationAroundAxis(T a, T b, T c)
	 {
	    Matrix4<T> ma, mb, mc;
	    float ac = cos(a);
	    float as = sin(a);
	    float bc = cos(b);
	    float bs = sin(b);
	    float cc = cos(c);
	    float cs = sin(c);

	    ma.at(1,1) = ac;
	    ma.at(2,1) = as;
	    ma.at(1,2) = -as;
	    ma.at(2,2) = ac;

	    mb.at(0,0) = bc;
	    mb.at(2,0) = -bs;
	    mb.at(0,2) = bs;
	    mb.at(2,2) = bc;

	    mc.at(0,0) = cc;
	    mc.at(1,0) = cs;
	    mc.at(0,1) = -cs;
	    mc.at(1,1) = cc;

	    /*cout << "RotVec = " << a << "," << b << "," << c << endl;
	      cout << "Rx = " << endl << ma;
	      cout << "Ry = " << endl << mb;
	      cout << "Rz = " << endl << mc;*/

	    Matrix4<T> ret = ma * mb * mc;
	    //cout << "Result = " << endl << ma * (mb * mc);

	    return ret;
	 }

	 /// Creates translation matrix
	 /**
	  * Creates translation matrix.
	  * @param x X-direction translation
	  * @param y Y-direction translation
	  * @param z Z-direction translation
	  * @param w for W-coordinate translation (impictily set to 1)
	  */
	 static Matrix4<T> createTranslation(T x, T y, T z, T w = 1)
	 {
	    Matrix4 ret;
	    ret.at(3,0) = x;
	    ret.at(3,1) = y;
	    ret.at(3,2) = z;
	    ret.at(3,3) = w;

	    return ret;
	 }


	 //---------------------[ equiality operators ]------------------------------
	 /**
	  * Equality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @note Test of equality is based of threshold EPSILON value. To be two
	  * values equal, must satisfy this condition all elements of matrix
	  * | lws[i] - rhs[i] | < EPSILON,
	  * same for y-coordinate, z-coordinate, and w-coordinate.
	  */
	 bool operator==(const Matrix4<T>& rhs) const
	 {
	    for (int i = 0; i < 16; i++)
	    {
	       if (fabs(data[i] - rhs.data[i]) >= EPSILON)
		  return false;
	    }
	    return true;
	 }

	 /**
	  * Inequality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @return not (lhs == rhs) :-P
	  */
	 bool operator!=(const Matrix4<T>& rhs) const
	 {
	    return !(*this == rhs);
	 }


	 //---------------------[ access operators ]---------------------------------
	 /**
	  * Get reference to element at postion (x,y).
	  * @param x Number of column (0..3)
	  * @param y Number of row (0..3)
	  */
	 T& at(int x, int y)
	 {
	    assert(x >= 0 && x < 4);
	    assert(y >= 0 && y < 4);
	    return data[x * 4 + y];
	 }

	 /**
	  * Get constant reference to element at postion (x,y).
	  * @param x Number of column (0..3)
	  * @param y Number of row (0..3)
	  */
	 const T& at(int x, int y) const
	 {
	    assert(x >= 0 && x < 4);
	    assert(y >= 0 && y < 4);
	    return data[x * 4 + y];
	 }


	 /**
	  * Sets translation part of matrix.
	  *
	  * @param v Vector of translation to be set.
	  */
	 void setTranslation(const Vector3<T>& v)
	 {
	    at(3,0) = v.x;
	    at(3,1) = v.y;
	    at(3,2) = v.z;
	    at(3,3) = 1;
	 }

	 Vector3<T> getTranslation()
	 { return Vector3<T>(at(3,0),at(3,1),at(3,2)); }

	 /**
	  * Sets roation part (matrix 3x3) of matrix.
	  *
	  * @param m Rotation part of matrix
	  */
	 void setRotation(const Matrix3<T>& m)
	 {
	    for (int i = 0; i < 3; i++)
	    {
	       for (int j = 0; j < 3; j++)
	       {
		  at(i,j) = m.at(i,j);
	       }
	    }
	 }


	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T>& operator=(const Matrix4<T>& rhs)
	 {
	    memcpy(data, rhs.data, sizeof(T) * 16);
	    return * this;
	 }

	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T>& operator=(const Matrix3<T>& rhs)
	 {
		 data[0] = rhs.data[0];
		 data[1] = rhs.data[1];
		 data[2] = rhs.data[2];
		 data[4] = rhs.data[3];
		 data[5] = rhs.data[4];
		 data[6] = rhs.data[5];
		 data[8] = rhs.data[6];
		 data[9] = rhs.data[7];
		 data[10] = rhs.data[8];
	    return * this;
	 }

	 /**
	  * Copies the rotational portion to a matrix 3
	  * @param mat3 Right hand side argument of binary operator.
	  */
	 void toMatrix3(Matrix3<T>& mat3) const {
		 mat3.data[0] = data[0];
		 mat3.data[1] = data[1];
		 mat3.data[2] = data[2];
		 mat3.data[3] = data[4];
		 mat3.data[4] = data[5];
		 mat3.data[5] = data[6];
		 mat3.data[6] = data[8];
		 mat3.data[7] = data[9];
		 mat3.data[8] = data[10];
	 }


	 /**
	  * Copy casting operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 template <class FromT>
	 Matrix4<T>& operator=(const Matrix4<FromT>& rhs)
	 {
	    for (int i = 0; i < 16; i++)
	    {
	       data[i] = static_cast<T>(rhs.data[i]);
	    }
	    return * this;
	 }

	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T>& operator=(const T* rhs)
	 {
	    memcpy(data, rhs, sizeof(T) * 16);
	    return * this;
	 }

	 /*Matrix4<T> & operator=(const double* m)
	   {
	   for (int i = 0; i < 16; i++) data[i] = (T)m[i];
	   return * this;
	   }*/

	 //--------------------[ matrix with matrix operations ]---------------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator+(const Matrix4<T>& rhs) const
	 {
	    Matrix4<T> ret;
	    for (int i = 0; i < 16; i++)
	       ret.data[i] = data[i] + rhs.data[i];
	    return ret;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator-(const Matrix4<T>& rhs) const
	 {
	    Matrix4<T> ret;
	    for (int i = 0; i < 16; i++)
	       ret.data[i] = data[i] - rhs.data[i];
	    return ret;
	 }

	 //--------------------[ matrix with scalar operations ]---------------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator+(T rhs) const
	 {
	    Matrix4<T> ret;
	    for (int i = 0; i < 16; i++)
	       ret.data[i] = data[i] + rhs;
	    return ret;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator-(T rhs) const
	 {
	    Matrix4<T> ret;
	    for (int i = 0; i < 16; i++)
	       ret.data[i] = data[i] - rhs;
	    return ret;
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator*(T rhs) const
	 {
	    Matrix4<T> ret;
	    for (int i = 0; i < 16; i++)
	       ret.data[i] = data[i] * rhs;
	    return ret;
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator/(T rhs) const
	 {
	    Matrix4<T> ret;
	    for (int i = 0; i < 16; i++)
	       ret.data[i] = data[i] / rhs;
	    return ret;
	 }


	 //--------------------[ multiply operators ]--------------------------------
	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector4<T> operator*(const Vector4<T>& rhs) const
	 {
	    return Vector4<T>(
	       data[0] * rhs.x + data[4] * rhs.y + data[8]  * rhs.z + data[12] * rhs.w,
	       data[1] * rhs.x + data[5] * rhs.y + data[9]  * rhs.z + data[13] * rhs.w,
	       data[2] * rhs.x + data[6] * rhs.y + data[10] * rhs.z + data[14] * rhs.w,
	       data[3] * rhs.x + data[7] * rhs.y + data[11] * rhs.z + data[15] * rhs.w
	       );

	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator*(const Vector3<T>& rhs) const
	 {
	    return Vector3<T>(
	       data[0] * rhs.x + data[4] * rhs.y + data[8] * rhs.z,
	       data[1] * rhs.x + data[5] * rhs.y + data[9] * rhs.z,
	       data[2] * rhs.x + data[6] * rhs.y + data[10] * rhs.z
	       );
	 }

	 /**
	  * Multiplication operator
	  *
	  * This 2D Vector operation translates, scales and rotates the vector
	  *
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector2<T> operator*(const Vector2<T>& rhs) const
	 {
	    return Vector2<T>(
	       data[0] * rhs.x + data[4] * rhs.y + data[12],
	       data[1] * rhs.x + data[5] * rhs.y + data[13]
	       );
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 AABB3<T> operator*(const AABB3<T>& rhs) const
	 {
	    return AABB3<T>(*this * rhs.pMin, *this * rhs.pMax);
	 }


	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix4<T> operator*(Matrix4<T>& rhs) const {
	    static Matrix4<T> w;
	    for (int i = 0; i < 4; i++) {
	       for (int j = 0; j < 4; j++) {
			  T n = 0;
			  for (int k = 0; k < 4; k++) {
				  n += rhs.at(i, k) * at(k, j);
			  }
			  w.at(i, j) = n;
	       }
	    }
	    return w;
	 }

	 //---------------------------[ misc operations ]----------------------------

	 /**
	  * Computes determinant of matrix
	  * @return Determinant of matrix
	  * @note This function does 3 * 4 * 6 mul, 3 * 6 add.
	  */
	 T det() {
	    return
	       + at(3,0) * at(2,1) * at(1,2) * at(0,3)
	       - at(2,0) * at(3,1) * at(1,2) * at(0,3)
	       - at(3,0) * at(1,1) * at(2,2) * at(0,3)
	       + at(1,0) * at(3,1) * at(2,2) * at(0,3)

	       + at(2,0) * at(1,1) * at(3,2) * at(0,3)
	       - at(1,0) * at(2,1) * at(3,2) * at(0,3)
	       - at(3,0) * at(2,1) * at(0,2) * at(1,3)
	       + at(2,0) * at(3,1) * at(0,2) * at(1,3)

	       + at(3,0) * at(0,1) * at(2,2) * at(1,3)
	       - at(0,0) * at(3,1) * at(2,2) * at(1,3)
	       - at(2,0) * at(0,1) * at(3,2) * at(1,3)
	       + at(0,0) * at(2,1) * at(3,2) * at(1,3)

	       + at(3,0) * at(1,1) * at(0,2) * at(2,3)
	       - at(1,0) * at(3,1) * at(0,2) * at(2,3)
	       - at(3,0) * at(0,1) * at(1,2) * at(2,3)
	       + at(0,0) * at(3,1) * at(1,2) * at(2,3)

	       + at(1,0) * at(0,1) * at(3,2) * at(2,3)
	       - at(0,0) * at(1,1) * at(3,2) * at(2,3)
	       - at(2,0) * at(1,1) * at(0,2) * at(3,3)
	       + at(1,0) * at(2,1) * at(0,2) * at(3,3)

	       + at(2,0) * at(0,1) * at(1,2) * at(3,3)
	       - at(0,0) * at(2,1) * at(1,2) * at(3,3)
	       - at(1,0) * at(0,1) * at(2,2) * at(3,3)
	       + at(0,0) * at(1,1) * at(2,2) * at(3,3);
	 }


	 /**
	  * Inverses this matrix
	  * @return This matrix, inversed
	  * @note This is a little bit time consuming operation
	  * (16 * 6 * 3 mul, 16 * 5 add + det() + mul() functions)
	  */
	 Matrix4<T> inverse() {
	    Matrix4<T> ret;

	    ret.at(0,0) =
	       + at(2,1) * at(3,2) * at(1,3)
	       - at(3,1) * at(2,2) * at(1,3)
	       + at(3,1) * at(1,2) * at(2,3)
	       - at(1,1) * at(3,2) * at(2,3)
	       - at(2,1) * at(1,2) * at(3,3)
	       + at(1,1) * at(2,2) * at(3,3);

	    ret.at(1,0) =
	       + at(3,0) * at(2,2) * at(1,3)
	       - at(2,0) * at(3,2) * at(1,3)
	       - at(3,0) * at(1,2) * at(2,3)
	       + at(1,0) * at(3,2) * at(2,3)
	       + at(2,0) * at(1,2) * at(3,3)
	       - at(1,0) * at(2,2) * at(3,3);

	    ret.at(2,0) =
	       + at(2,0) * at(3,1) * at(1,3)
	       - at(3,0) * at(2,1) * at(1,3)
	       + at(3,0) * at(1,1) * at(2,3)
	       - at(1,0) * at(3,1) * at(2,3)
	       - at(2,0) * at(1,1) * at(3,3)
	       + at(1,0) * at(2,1) * at(3,3);

	    ret.at(3,0) =
	       + at(3,0) * at(2,1) * at(1,2)
	       - at(2,0) * at(3,1) * at(1,2)
	       - at(3,0) * at(1,1) * at(2,2)
	       + at(1,0) * at(3,1) * at(2,2)
	       + at(2,0) * at(1,1) * at(3,2)
	       - at(1,0) * at(2,1) * at(3,2);

	    ret.at(0,1) =
	       + at(3,1) * at(2,2) * at(0,3)
	       - at(2,1) * at(3,2) * at(0,3)
	       - at(3,1) * at(0,2) * at(2,3)
	       + at(0,1) * at(3,2) * at(2,3)
	       + at(2,1) * at(0,2) * at(3,3)
	       - at(0,1) * at(2,2) * at(3,3);

	    ret.at(1,1) =
	       + at(2,0) * at(3,2) * at(0,3)
	       - at(3,0) * at(2,2) * at(0,3)
	       + at(3,0) * at(0,2) * at(2,3)
	       - at(0,0) * at(3,2) * at(2,3)
	       - at(2,0) * at(0,2) * at(3,3)
	       + at(0,0) * at(2,2) * at(3,3);

	    ret.at(2,1) =
	       + at(3,0) * at(2,1) * at(0,3)
	       - at(2,0) * at(3,1) * at(0,3)
	       - at(3,0) * at(0,1) * at(2,3)
	       + at(0,0) * at(3,1) * at(2,3)
	       + at(2,0) * at(0,1) * at(3,3)
	       - at(0,0) * at(2,1) * at(3,3);

	    ret.at(3,1) =
	       + at(2,0) * at(3,1) * at(0,2)
	       - at(3,0) * at(2,1) * at(0,2)
	       + at(3,0) * at(0,1) * at(2,2)
	       - at(0,0) * at(3,1) * at(2,2)
	       - at(2,0) * at(0,1) * at(3,2)
	       + at(0,0) * at(2,1) * at(3,2);

	    ret.at(0,2) =
	       + at(1,1) * at(3,2) * at(0,3)
	       - at(3,1) * at(1,2) * at(0,3)
	       + at(3,1) * at(0,2) * at(1,3)
	       - at(0,1) * at(3,2) * at(1,3)
	       - at(1,1) * at(0,2) * at(3,3)
	       + at(0,1) * at(1,2) * at(3,3);

	    ret.at(1,2) =
	       + at(3,0) * at(1,2) * at(0,3)
	       - at(1,0) * at(3,2) * at(0,3)
	       - at(3,0) * at(0,2) * at(1,3)
	       + at(0,0) * at(3,2) * at(1,3)
	       + at(1,0) * at(0,2) * at(3,3)
	       - at(0,0) * at(1,2) * at(3,3);

	    ret.at(2,2) =
	       + at(1,0) * at(3,1) * at(0,3)
	       - at(3,0) * at(1,1) * at(0,3)
	       + at(3,0) * at(0,1) * at(1,3)
	       - at(0,0) * at(3,1) * at(1,3)
	       - at(1,0) * at(0,1) * at(3,3)
	       + at(0,0) * at(1,1) * at(3,3);

	    ret.at(3,2) =
	       + at(3,0) * at(1,1) * at(0,2)
	       - at(1,0) * at(3,1) * at(0,2)
	       - at(3,0) * at(0,1) * at(1,2)
	       + at(0,0) * at(3,1) * at(1,2)
	       + at(1,0) * at(0,1) * at(3,2)
	       - at(0,0) * at(1,1) * at(3,2);

	    ret.at(0,3) =
	       + at(2,1) * at(1,2) * at(0,3)
	       - at(1,1) * at(2,2) * at(0,3)
	       - at(2,1) * at(0,2) * at(1,3)
	       + at(0,1) * at(2,2) * at(1,3)
	       + at(1,1) * at(0,2) * at(2,3)
	       - at(0,1) * at(1,2) * at(2,3);

	    ret.at(1,3) =
	       + at(1,0) * at(2,2) * at(0,3)
	       - at(2,0) * at(1,2) * at(0,3)
	       + at(2,0) * at(0,2) * at(1,3)
	       - at(0,0) * at(2,2) * at(1,3)
	       - at(1,0) * at(0,2) * at(2,3)
	       + at(0,0) * at(1,2) * at(2,3);

	    ret.at(2,3) =
	       + at(2,0) * at(1,1) * at(0,3)
	       - at(1,0) * at(2,1) * at(0,3)
	       - at(2,0) * at(0,1) * at(1,3)
	       + at(0,0) * at(2,1) * at(1,3)
	       + at(1,0) * at(0,1) * at(2,3)
	       - at(0,0) * at(1,1) * at(2,3);

	    ret.at(3,3) =
	       + at(1,0) * at(2,1) * at(0,2)
	       - at(2,0) * at(1,1) * at(0,2)
	       + at(2,0) * at(0,1) * at(1,2)
	       - at(0,0) * at(2,1) * at(1,2)
	       - at(1,0) * at(0,1) * at(2,2)
	       + at(0,0) * at(1,1) * at(2,2);

	    return ret * (1.0f / det());
	 }

	 /**
	  * Computes determinant of the 3x3 subset of this matrix
	  * @return Determinant of the 3x3 subset of this matrix
	  */
	 T det3() {
	    return
	    	at(0,0)*at(1,1)*at(2,2)
	    	+at(0,1)*at(1,2)*at(2,0)
	    	+at(0,2)*at(1,0)*at(2,1)
	    	-at(0,0)*at(1,2)*at(2,1)
	    	-at(0,1)*at(1,0)*at(2,2)
	    	-at(0,2)*at(1,1)*at(2,0);
	 }


	 /**
	  * Computes inverse matrix of 3x3 subset
	  * @return Inverse matrix of 3x3 subset of this matrix plus the rest untouched
	  */
	 Matrix4<T> inverse3() {
	    Matrix4<T> ret = *this;
	    T detMult = 1.0f / det3();
	    ret.at(0,0) = (at(1,1)*at(2,2) - at(2,1)*at(1,2)) * detMult;
	    ret.at(0,1) = (at(2,1)*at(0,2) - at(0,1)*at(2,2)) * detMult;
	    ret.at(0,2) = (at(0,1)*at(1,2) - at(1,1)*at(0,2)) * detMult;
	    ret.at(1,0) = (at(2,0)*at(1,2) - at(1,0)*at(2,2)) * detMult;
	    ret.at(1,1) = (at(0,0)*at(2,2) - at(2,0)*at(0,2)) * detMult;
	    ret.at(1,2) = (at(1,0)*at(0,2) - at(0,0)*at(1,2)) * detMult;
	    ret.at(2,0) = (at(1,0)*at(2,1) - at(2,0)*at(1,1)) * detMult;
	    ret.at(2,1) = (at(2,0)*at(0,1) - at(0,0)*at(2,1)) * detMult;
	    ret.at(2,2) = (at(0,0)*at(1,1) - at(1,0)*at(0,1)) * detMult;
	    return ret;
	 }

	 /**
	  * Scales the matrix by x, y and z
	  *
	  * Does not scale the translational component
	  *
	  * Modifies matrix in-place
	  */
	 void scale(T sx, T sy, T sz) {
		 data[0] *= sx;
		 data[1] *= sx;
		 data[2] *= sx;
		 data[3] *= sx;
		 data[4] *= sy;
		 data[5] *= sy;
		 data[6] *= sy;
		 data[7] *= sy;
		 data[8] *= sz;
		 data[9] *= sz;
		 data[10] *= sz;
		 data[11] *= sz;
	 }

	 /**
	  * Translates the matrix by x, y and z
	  * Modifies matrix in-place
	  */
	 void translate(T tx, T ty, T tz) {
	     data[12] += (data[0] * tx + data[4] * ty + data[8] * tz);
	     data[13] += (data[1] * tx + data[5] * ty + data[9] * tz);
	     data[14] += (data[2] * tx + data[6] * ty + data[10] * tz);
	     data[15] += (data[3] * tx + data[7] * ty + data[11] * tz);
	 }

	 /**
	  * Rotates the matrix by angle (in degrees) about the axis specified as x, y and z
	  *
	  * Does not rotate the translational component
	  *
	  * Modifies matrix in-place
	  */
	 void rotate(T angle, T x, T y, T z) {
	    T sinAngle, cosAngle;
	    T mag = sqrtf(x * x + y * y + z * z);
	    sinAngle = sinf ( angle * PI / 180.0f );
	    cosAngle = cosf ( angle * PI / 180.0f );
	    if ( mag > 0.0f ) {
	       T xx, yy, zz, xy, yz, zx, xs, ys, zs;
	       T oneMinusCos;
	       // constructed with identity
	       Matrix4<T> rotMat;
	       x /= mag;
	       y /= mag;
	       z /= mag;
	       xx = x * x;
	       yy = y * y;
	       zz = z * z;
	       xy = x * y;
	       yz = y * z;
	       zx = z * x;
	       xs = x * sinAngle;
	       ys = y * sinAngle;
	       zs = z * sinAngle;
	       oneMinusCos = 1.0f - cosAngle;
	       rotMat.data[0] = (oneMinusCos * xx) + cosAngle;
	       rotMat.data[1] = (oneMinusCos * xy) - zs;
	       rotMat.data[2] = (oneMinusCos * zx) + ys;

	       rotMat.data[4] = (oneMinusCos * xy) + zs;
	       rotMat.data[5] = (oneMinusCos * yy) + cosAngle;
	       rotMat.data[6] = (oneMinusCos * yz) - xs;

	       rotMat.data[8] = (oneMinusCos * zx) - ys;
	       rotMat.data[9] = (oneMinusCos * yz) + xs;
	       rotMat.data[10] = (oneMinusCos * zz) + cosAngle;
	       rotMat.data[11] = 0.0F;
	       // gl does mult on rhs
	       *this = *this * rotMat;
	    }
	 }

	 void frustum(T left, T right, T bottom, T top, T nearZ, T farZ) {
	     T deltaX = right - left;
	     T deltaY = top - bottom;
	     T deltaZ = farZ - nearZ;
	     // constructed with identity
	     Matrix4<T>    frust;

	     if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
	          (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
	          return;

	     frust.data[0] = 2.0f * nearZ / deltaX;
	     frust.data[1] = frust.data[2] = frust.data[3] = 0.0f;

	     frust.data[5] = 2.0f * nearZ / deltaY;
	     frust.data[4] = frust.data[6] = frust.data[7] = 0.0f;

	     frust.data[8] = (right + left) / deltaX;
	     frust.data[9] = (top + bottom) / deltaY;
	     frust.data[10] = -(nearZ + farZ) / deltaZ;
	     frust.data[11] = -1.0f;

	     frust.data[14] = -2.0f * nearZ * farZ / deltaZ;
	     frust.data[12] = frust.data[13] = frust.data[15] = 0.0f;
	     *this = *this * frust;
	 }

	 void perspective(T fovy, T aspect, T nearZ, T farZ) {
	    T frustumW, frustumH;
	    frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
	    frustumW = frustumH * aspect;
	    frustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
	 }

	 void ortho(T left, T right, T bottom, T top, T nearZ, T farZ) {
	     T deltaX = right - left;
	     T deltaY = top - bottom;
	     T deltaZ = farZ - nearZ;
	     // constructed with identity
	     Matrix4<T> ortho;
	     if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) ) {
	         return;
	     }
	     ortho.data[0] = 2.0f / deltaX;
	     ortho.data[12] = -(right + left) / deltaX;
	     ortho.data[5] = 2.0f / deltaY;
	     ortho.data[13] = -(top + bottom) / deltaY;
	     ortho.data[10] = -2.0f / deltaZ;
	     ortho.data[14] = -(nearZ + farZ) / deltaZ;
	     *this = *this * ortho;
	 }

	 void lookAt(Vector3<T> eye,
	 		Vector3<T> center,
	 		Vector3<T> origUp) {
		 Vector3<T> forward, side, up;
	     forward = center - eye;

	     up = origUp;
	     forward.normalize();
	     side = forward.cross(up);
	     side.normalize();
	     up = side.cross(forward);

         data[0] = side[0];
         data[1] = up[0];
         data[2] = -forward[0];
         data[4] = side[1];
         data[5] = up[1];
         data[6] = -forward[1];
         data[8] = side[2];
         data[9] = up[2];
         data[10] = -forward[2];
         translate(-eye.x, -eye.y, -eye.z);
	 }

	 /**
	  * Transpose matrix.
	  */
	 Matrix4<T> transpose()
	 {
	    return Matrix4<T>(data[0], data[4], data[8], data[12],
	    		data[1], data[5], data[9], data[13],
	    		data[2], data[6], data[10], data[14],
	    		data[3], data[7], data[11], data[15]);
	 }

	 /**
	  * Linear interpolation of two vectors
	  * @param fact Factor of interpolation. For translation from positon
	  * of this matrix (lhs) to matrix rhs, values of factor goes from 0.0 to 1.0.
	  * @param rhs Second Matrix for interpolation
	  * @note Hovewer values of fact parameter are reasonable only in interval
	  * [0.0 , 1.0], you can pass also values outside of this interval and you
	  * can get result (extrapolation?)
	  */
	 Matrix4<T> lerp(T fact, const Matrix4<T>& rhs) const
	 {
	    Matrix4<T> ret = (*this) + (rhs - (*this)) * fact;
	    return ret;
	 }


	 //-------------[ conversion ]-----------------------------
	 /**
	  * Conversion to pointer operator
	  * @return Pointer to internaly stored (in managment of class Matrix4<T>)
	  * used for passing Matrix4<T> values to gl*[fd]v functions.
	  */
	 operator T*(){ return (T*) data; }

	 /**
	  * Conversion to pointer operator
	  * @return Constant Pointer to internaly stored (in managment of class Matrix4<T>)
	  * used for passing Matrix4<T> values to gl*[fd]v functions.
	  */
	 operator const T*() const { return (const T*) data; }

	 //----------[ output operator ]----------------------------
	 /**
	  * Output to stream operator
	  * @param lhs Left hand side argument of operator (commonly ostream instance).
	  * @param rhs Right hand side argument of operator.
	  * @return Left hand side argument - the ostream object passed to operator.
	  */
	 /*
	 friend ostream& operator << (ostream& lhs, const Matrix4<T>& rhs)
	 {
	    for (int i = 0; i < 4; i++)
	    {
	       lhs << "|\t";
	       for (int j = 0; j < 4; j++)
	       {
		  lhs << rhs.at(j,i) << "\t";
	       }
	       lhs << "|" << endl;
	    }
	    return lhs;
	 }*/

   };


   /// Matrix 3x3 of floats
   typedef Matrix4<float> Matrix4f;
   /// Matrix 3x3 of doubles
   typedef Matrix4<double> Matrix4d;

}

#endif /* MATRIX4_H_ */
