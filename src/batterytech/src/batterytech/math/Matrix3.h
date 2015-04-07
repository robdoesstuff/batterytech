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
// Name        : Matrix3.h
// Description : 3x3 Matrix Structure and Operations
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

#ifndef MATRIX3_H_
#define MATRIX3_H_

#include <math.h>
#include <string.h>
#include <assert.h>
#include "Vector3.h"
#include "Matrix4.h"

#ifndef DEG2RAD
#define DEG2RAD(x) ((x * M_PI) / 180.0)
   //#define EPSILON (4.37114e-07)

   const double epsilon = 4.37114e-05;
#endif
#ifndef EPSILON
#define EPSILON epsilon
#endif

namespace BatteryTech {

/**
    * Class for matrix 3x3.
    * @note Data stored in this matrix are in column major order.
    */
   template <class T>
   struct Matrix3
   {
      public:
	 /// Data stored in column major order
	 T data[9];

	 //--------------------------[ constructors ]-------------------------------
	 /**
	  * Creates identity matrix
	  */
	 Matrix3()
	 {
	    for (int i = 0; i < 9; i++)
	       data[i] = (i % 4) ? 0 : 1;
	 }

	 /**
	  * Copy matrix values from array (these data must be in column
	  * major order!)
	  */
	 Matrix3(const T * dt)
	 {
	    memcpy(data, dt, sizeof(T) * 9);
	 }

	 /**
	  * Copy constructor.
	  * @param src Data source for new created instance of Matrix3
	  */
	 Matrix3(const Matrix3<T>& src)
	 {
	    memcpy(data, src.data, sizeof(T) * 9);
	 }

	 /**
	  * Copy casting constructor.
	  * @param src Data source for new created instance of Matrix3
	  */
	 template<class FromT>
	 Matrix3(const Matrix3<FromT>& src)
	 {
	    for (int i = 0; i < 9; i++)
	    {
	       data[i] = static_cast<T>(src.data[i]);
	    }
	 }

	 /**
	  * Resets matrix to be identity matrix
	  */
	 void identity()
	 {
	    for (int i = 0; i < 9; i++)
	       data[i] = (i % 4) ? 0 : 1;
	 }



	 /**
	  * Creates rotation matrix by rotation around axis.
	  * @param a Angle (in radians) of rotation around axis X.
	  * @param b Angle (in radians) of rotation around axis Y.
	  * @param c Angle (in radians) of rotation around axis Z.
	  */
	 static Matrix3<T> createRotationAroundAxis(T a, T b, T c)
	 {
	    Matrix3<T> ma, mb, mc;
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

	    Matrix3<T> ret = ma * mb * mc;
	    return ret;
	 }

	 /**
	  * Creates roation matrix from ODE Matrix.
	  */
	 template <class It>
	 static Matrix3<T> fromOde(const It* mat)
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 3; i++)
	    {
	       for (int j = 0; j < 3; j++)
	       {
		  ret.at(i,j) = static_cast<T>(mat[j * 4 + i]);
	       }
	    }
	    return ret;
	 }


	 //---------------------[ equality operators ]------------------------------
	 /**
	  * Equality test operator
	  * @param rhs Right hand side argument of binary operator.
	  * @note Test of equality is based of threshold EPSILON value. To be two
	  * values equal, must satisfy this condition all elements of matrix
	  * | lws[i] - rhs[i] | < EPSILON,
	  * same for y-coordinate, z-coordinate, and w-coordinate.
	  */
	 bool operator==(const Matrix3<T>& rhs) const
	 {
	    for (int i = 0; i < 9; i++)
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
	 bool operator!=(const Matrix3<T>& rhs) const
	 {
	    return !(*this == rhs);
	 }


	 //---------------------[ access operators ]---------------------------------
	 /**
	  * Get reference to element at postion (x,y).
	  * @param x Number of column (0..2)
	  * @param y Number of row (0..2)
	  */
	 T& at(int x, int y)
	 {
	    assert(x >= 0 && x < 3);
	    assert(y >= 0 && y < 3);
	    return data[x * 3 + y];
	 }

	 /**
	  * Get constant reference to element at postion (x,y).
	  * @param x Number of column (0..2)
	  * @param y Number of row (0..2)
	  */
	 const T& at(int x, int y) const
	 {
	    assert(x >= 0 && x < 3);
	    assert(y >= 0 && y < 3);
	    return data[x * 3 + y];
	 }

	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T>& operator=(const Matrix3<T>& rhs)
	 {
	    memcpy(data, rhs.data, sizeof(T) * 9);
	    return * this;
	 }


	 /**
	  * Copy casting operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 template<class FromT>
	 Matrix3<T>& operator=(const Matrix3<FromT>& rhs)
	 {
	    for (int i = 0; i < 9; i++)
	    {
	       data[i] = static_cast<T>(rhs.data[i]);
	    }
	    return * this;
	 }

	 /**
	  * Copy operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T>& operator=(const T* rhs)
	 {
	    memcpy(data, rhs, sizeof(T) * 9);
	    return * this;
	 }


	 /*Matrix3<T> & operator=(const double* m)
	   {
	   for (int i = 0; i < 9; i++) data[i] = (T)m[i];
	   return * this;
	   }*/

	 //--------------------[ matrix with matrix operations ]---------------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator+(const Matrix3<T>& rhs) const
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 9; i++)
	       ret.data[i] = data[i] + rhs.data[i];
	    return ret;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator-(const Matrix3<T>& rhs) const
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 9; i++)
	       ret.data[i] = data[i] - rhs.data[i];
	    return ret;
	 }

	 //--------------------[ matrix with scalar operations ]---------------------
	 /**
	  * Addition operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator+(T rhs) const
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 9; i++)
	       ret.data[i] = data[i] + rhs;
	    return ret;
	 }

	 /**
	  * Substraction operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator-(T rhs) const
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 9; i++)
	       ret.data[i] = data[i] - rhs;
	    return ret;
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator*(T rhs) const
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 9; i++)
	       ret.data[i] = data[i] * rhs;
	    return ret;
	 }

	 /**
	  * Division operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator/(T rhs) const
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 9; i++)
	       ret.data[i] = data[i] / rhs;
	    return ret;
	 }


	 //--------------------[ multiply operators ]--------------------------------
	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Vector3<T> operator*(const Vector3<T>& rhs) const
	 {
	    return Vector3<T>(
	       data[0] * rhs.x + data[3] * rhs.y + data[6] * rhs.z,
	       data[1] * rhs.x + data[4] * rhs.y + data[7] * rhs.z,
	       data[2] * rhs.x + data[5] * rhs.y + data[8] * rhs.z
	       );
	 }

	 /**
	  * Multiplication operator
	  * @param rhs Right hand side argument of binary operator.
	  */
	 Matrix3<T> operator*(Matrix3<T> rhs) const {
	    static Matrix3<T> w;
	    for (int i = 0; i < 3; i++) {
	       for (int j = 0; j < 3; j++) {
			  T n = 0;
			  for (int k = 0; k < 3; k++) {
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
	  */
	 T det()
	 {
	    return
	    	at(0,0)*at(1,1)*at(2,2)
	    	+at(0,1)*at(1,2)*at(2,0)
	    	+at(0,2)*at(1,0)*at(2,1)
	    	-at(0,0)*at(1,2)*at(2,1)
	    	-at(0,1)*at(1,0)*at(2,2)
	    	-at(0,2)*at(1,1)*at(2,0)
	    	;
	 }


	 /**
	  * Computes inverse matrix
	  * @return Inverse matrix of this matrix.
	  */
	 Matrix3<T> inverse()
	 {
	    Matrix3<T> ret;
	    ret.at(0,0) = at(1,1)*at(2,2) - at(2,1)*at(1,2);
	    ret.at(0,1) = at(2,1)*at(0,2) - at(0,1)*at(2,2);
	    ret.at(0,2) = at(0,1)*at(1,2) - at(1,1)*at(0,2);
	    ret.at(1,0) = at(2,0)*at(1,2) - at(1,0)*at(2,2);
	    ret.at(1,1) = at(0,0)*at(2,2) - at(2,0)*at(0,2);
	    ret.at(1,2) = at(1,0)*at(0,2) - at(0,0)*at(1,2);
	    ret.at(2,0) = at(1,0)*at(2,1) - at(2,0)*at(1,1);
	    ret.at(2,1) = at(2,0)*at(0,1) - at(0,0)*at(2,1);
	    ret.at(2,2) = at(0,0)*at(1,1) - at(1,0)*at(0,1);
	    return ret * (1.0f / det());
	 }

	 /**
	  * Transpose matrix.
	  */
	 Matrix3<T> transpose()
	 {
	    Matrix3<T> ret;
	    for (int i = 0; i < 3; i++)
	    {
	       for (int j = 0; j < 3; j++)
	       {
		  ret.at(i,j) = at(j,i);
	       }
	    }
	    return ret;
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
	 Matrix3<T> lerp(T fact, const Matrix3<T>& rhs) const
	 {
	    Matrix3<T> ret = (*this) + (rhs - (*this)) * fact;
	    return ret;
	 }


	 //-------------[ conversion ]-----------------------------

	 /**
	  * Conversion to pointer operator
	  * @return Pointer to internaly stored (in managment of class Matrix3<T>)
	  * used for passing Matrix3<T> values to gl*[fd]v functions.
	  */
	 operator T*(){ return (T*) data; }

	 /**
	  * Conversion to pointer operator
	  * @return Constant Pointer to internaly stored (in managment of class Matrix3<T>)
	  * used for passing Matrix3<T> values to gl*[fd]v functions.
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
	 friend ostream& operator << (ostream& lhs, const Matrix3<T>& rhs)
	 {
	    for (int i = 0; i < 3; i++)
	    {
	       lhs << "|\t";
	       for (int j = 0; j < 3; j++)
	       {
		  lhs << rhs.at(j,i) << "\t";
	       }
	       lhs << "|" << endl;
	    }
	    return lhs;
	 }*/

   };

   /// Matrix 3x3 of floats
   typedef Matrix3<float> Matrix3f;
   /// Matrix 3x3 of doubles
   typedef Matrix3<double> Matrix3d;

}

#endif /* MATRIX3_H_ */
