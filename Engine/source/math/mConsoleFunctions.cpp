//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "core/strings/stringFunctions.h"

#include "console/console.h"
#include "math/mMathFn.h"
#include "math/mRandom.h"
#include "math/mMath.h"
#include "math/mathUtils.h"

#include "console/engineAPI.h"


DefineEngineFunction( mSolveQuadratic, const char*, ( F32 a, F32 b, F32 c ),,
    "Solve a quadratic equation (2nd degree polynomial) of form a*x^2 + b*x + c = 0.\n"
    "@param a First Coefficient."
    "@param b Second Coefficient."
    "@param c Third Coefficient."
    "@returns A triple, containing: (sol x0 x1). (sol) is the number of solutions"
    "(being 0, 1, or 2), and (x0) and (x1) are the solutions, if any."
    "@ingroup Math" )
{
   F32 x[2];
   U32 sol = mSolveQuadratic( a, b, c, x );

   static const U32 bufSize = 256;
   char * retBuffer = Con::getReturnBuffer(bufSize);
   dSprintf(retBuffer, bufSize, "%d %g %g", sol, x[0], x[1]);
   return retBuffer;
}

DefineEngineFunction( mSolveCubic, const char*, ( F32 a, F32 b, F32 c, F32 d ),,
    "Solve a cubic equation (3rd degree polynomial) of form a*x^3 + b*x^2 + c*x + d = 0.\n"
    "@param a First Coefficient."
    "@param b Second Coefficient."
    "@param c Third Coefficient."
    "@param d Fourth Coefficient."
    "@returns A 4-tuple, containing: (sol x0 x1 x2). (sol) is the number of solutions"
    "(being 0, 1, 2 or 3), and (x0), (x1) and (x2) are the solutions, if any."
    "@ingroup Math" )
{    
   F32 x[3];
   U32 sol = mSolveCubic( a, b, c, d, x );

   static const U32 bufSize = 256;
   char * retBuffer = Con::getReturnBuffer(bufSize);
   dSprintf(retBuffer, bufSize, "%d %g %g %g", sol, x[0], x[1], x[2]);
   return retBuffer;
}

DefineEngineFunction( mSolveQuartic, const char*, ( F32 a, F32 b, F32 c, F32 d, F32 e ),,
    "Solve a quartic equation (4th degree polynomial) of form a*x^4 + b*x^3 + c*x^2 + d*x + e = 0.\n"
    "@param a First Coefficient."
    "@param b Second Coefficient."
    "@param c Third Coefficient."
    "@param d Fourth Coefficient."
    "@param e Fifth Coefficient."
    "@returns A 5-tuple, containing: (sol x0 x1 x2 c3). (sol) is the number of solutions"
    "(being 0, 1, 2, 3 or 4), and (x0), (x1), (x2) and (x3) are the solutions, if any."
    "@ingroup Math" )
{
   F32 x[4];
   static const U32 bufSize = 256;
   char * retBuffer = Con::getReturnBuffer(bufSize);
   U32 sol = mSolveQuartic(a, b, c, d, e, x);
   dSprintf(retBuffer, bufSize, "%d %g %g %g %g", sol, x[0], x[1], x[2], x[3]);
   return retBuffer;
}

DefineEngineFunction( mFloor, S32, ( F32 v ),,
    "Round v down to the nearest integer.\n"
    "@param v Number to convert to integer."
    "@returns Number converted to integer."
    "@ingroup Math" )
{
   return (S32)mFloor( v );
}

DefineEngineFunction( mRound, S32, ( F32 v  ),,
    "Round v to the nth decimal place or the nearest whole number by default."
    "@param v Value to roundn"
    "@return The rounded value as a S32."  
    "@ingroup Math" )  
{
   return mRound(v);
}

DefineEngineFunction(mRoundF, F32, (F32 v, F32 step), ,
   "Round v to the nth decimal place or the nearest whole number by default."
   "@param v Value to roundn"
   "@return The rounded value as a F32."
   "@ingroup Math")
{
   return mRoundF(v, step);
}

DefineEngineFunction(mRoundDelta, S32, (F32 v, S32 d), (0.0, 1),
   "Round v to the nearest number based on the delta"
   "@param v Value to round"
   "@param d Delta use when rounding"
   "@return The rounded value as a S32."
   "@ingroup Math")
{
   return (mFloor(v / d + 0.5) * d);
}


DefineEngineFunction( mRoundColour, F32, ( F32 v, S32 n ), (0),
   "Round v to the nth decimal place or the nearest whole number by default."
   "@param v Value to roundn"
   "@param n Number of decimal places to round to, 0 by defaultn"
   "@return The rounded value as a S32."
   "@ingroup Math")
{
   if (n <= 0)
      return mRound(v);
   else
      return mRound(v, n);
}

DefineEngineFunction( mCeil, S32, ( F32 v ),,
    "Round v up to the nearest integer.\n"
    "@param v Number to convert to integer."
    "@returns Number converted to integer."
    "@ingroup Math" )
{
   return (S32)mCeil( v );
}

DefineEngineFunction( mFloatLength, const char*, ( F32 v, U32 precision ),,
    "Formats the specified number to the given number of decimal places.\n"
    "@param v Number to format."
    "@param precision Number of decimal places to format to (1-9)."
    "@returns Number formatted to the specified number of decimal places."
    "@ingroup Math" )
{
   char fmtString[8] = "%.9f";

   if (precision >= 9)
      precision = 9;
   fmtString[2] = '0' + precision;

   static const U32 bufSize = 256;
   char * outBuffer = Con::getReturnBuffer(bufSize);
   dSprintf(outBuffer, bufSize, fmtString, v);
   return outBuffer;
}

//------------------------------------------------------------------------------

DefineEngineFunction( mAbs, F32, ( F32 v ),,
    "Calculate absolute value of specified value.\n"
    "@param v Input Value."
    "@returns Absolute value of specified value."
    "@ingroup Math" )
{
   return mFabs( v );
}

DefineEngineFunction( mFMod, F32, ( F32 v, F32 d ),,
    "Calculate the remainder of v/d.\n"
    "@param v Input Value."
    "@param d Divisor Value."
    "@returns The remainder of v/d."
    "@ingroup Math" )
{
   return mFmod( v, d );
}

DefineEngineFunction( mSqrt, F32, ( F32 v ),,
    "Calculate the square-root of v.\n"
    "@param v Input Value."
    "@returns The square-root of the input value."
    "@ingroup Math" )
{
   return mSqrt (v );
}

DefineEngineFunction( mPow, F32, ( F32 v, F32 p ),,
    "Calculate b raised to the p-th power.\n"
    "@param v Input Value."
    "@param p Power to raise value by."
    "@returns v raised to the p-th power."
    "@ingroup Math" )
{
   return mPow( v, p );
}

DefineEngineFunction( mLog, F32, ( F32 v ),,
    "Calculate the natural logarithm of v.\n"
    "@param v Input Value."
    "@returns The natural logarithm of the input value."
    "@ingroup Math" )
{
   return mLog( v );
}

DefineEngineFunction( mSin, F32, ( F32 v ),,
    "Calculate the sine of v.\n"
    "@param v Input Value (in radians)."
    "@returns The sine of the input value."
    "@ingroup Math" )
{
   return mSin( v );
}

DefineEngineFunction( mCos, F32, ( F32 v ),,
    "Calculate the cosine of v.\n"
    "@param v Input Value (in radians)."
    "@returns The cosine of the input value."
    "@ingroup Math" )
{
   return mCos( v );
}

DefineEngineFunction( mTan, F32, ( F32 v ),,
    "Calculate the tangent of v.\n"
    "@param v Input Value (in radians)."
    "@returns The tangent of the input value."
    "@ingroup Math" )
{
   return mTan( v );
}

DefineEngineFunction( mAsin, F32, ( F32 v ),,
    "Calculate the arc-sine of v.\n"
    "@param v Input Value (in radians)."
    "@returns The arc-sine of the input value."
    "@ingroup Math" )
{
   return mAsin( v );
}

DefineEngineFunction( mAcos, F32, ( F32 v ),,
    "Calculate the arc-cosine of v.\n"
    "@param v Input Value (in radians)."
    "@returns The arc-cosine of the input value."
    "@ingroup Math" )
{
   return mAcos( v );
}

DefineEngineFunction( mAtan, F32, ( F32 rise, F32 run ),,
    "Calculate the arc-tangent (slope) of a line defined by rise and run.\n"
    "@param rise of line."
    "@param run of line."
    "@returns The arc-tangent (slope) of a line defined by rise and run."
    "@ingroup Math" )
{
   return mAtan2( rise, run );
}

DefineEngineFunction( mRadToDeg, F32, ( F32 radians ),,
    "Convert specified radians into degrees.\n"
    "@param radians Input Value (in radians)."
    "@returns The specified radians value converted to degrees."
    "@ingroup Math" )
{
   return mRadToDeg( radians );
}

DefineEngineFunction( mDegToRad, F32, ( F32 degrees ),,
    "Convert specified degrees into radians.\n"
    "@param degrees Input Value (in degrees)."
    "@returns The specified degrees value converted to radians."
    "@ingroup Math" )
{
   return mDegToRad( degrees );
}

DefineEngineFunction( mClamp, F32, ( F32 v, F32 min, F32 max ),,
    "Clamp the specified value between two bounds.\n"
    "@param v Input value."
    "@param min Minimum Bound."
    "@param max Maximum Bound."
    "@returns The specified value clamped to the specified bounds."
    "@ingroup Math" )
{
   return mClampF( v, min, max );
}

DefineEngineFunction( mSaturate, F32, ( F32 v ),,
    "Clamp the specified value between 0 and 1 (inclusive).\n"
    "@param v Input value."
    "@returns The specified value clamped between 0 and 1 (inclusive)."
    "@ingroup Math" )
{
   return mClampF( v, 0.0f, 1.0f );
}

DefineEngineFunction(mWrapF, F32, (F32 v, F32 min, F32 max), ,
	"Wrap the specified value between two bounds.\n"
	"@param v Input value."
	"@param min Minimum Bound."
	"@param max Maximum Bound."
	"@returns The specified value wrapped to the specified bounds."
	"@ingroup Math")
{
	return mWrapF(v, min, max);
}

DefineEngineFunction(mWrap, S32, (S32 v, S32 min, S32 max), ,
	"Wrap the specified value between two bounds.\n"
	"@param v Input value."
	"@param min Minimum Bound."
	"@param max Maximum Bound."
	"@returns The specified value wrapped to the specified bounds."
	"@ingroup Math")
{
	return mWrap(v, min, max);
}


DefineEngineFunction( getMax, F32, ( F32 v1, F32 v2 ),,
    "Calculate the greater of two specified numbers.\n"
    "@param v1 Input value."
    "@param v2 Input value."
    "@returns The greater value of the two specified values."
    "@ingroup Math" )
{
   return getMax( v1, v2 );
}

DefineEngineFunction( getMin, F32, ( F32 v1, F32 v2 ),,
    "Calculate the lesser of two specified numbers.\n"
    "@param v1 Input value."
    "@param v2 Input value."
    "@returns The lesser value of the two specified values."
    "@ingroup Math" )
{
   return getMin( v1, v2 );
}

DefineEngineFunction( mLerp, F32, ( F32 v1, F32 v2, F32 time ),,
    "Calculate linearly interpolated value between two specified numbers using specified normalized time.\n"
    "@param v1 Interpolate From Input value."
    "@param v2 Interpolate To Input value."
    "@param time Normalized time used to interpolate values (0-1)."
    "@returns The interpolated value between the two specified values at normalized time t."
    "@ingroup Math" )
{
   return mLerp( v1, v2, time );
}

DefineEngineFunction(mInvLerp, F32, (F32 v1, F32 v2, F32 point), ,
   "Calculate the percentage of a point along a line.\n"
   "@param v1 Interpolate From Input value."
   "@param v2 Interpolate To Input value."
   "@param point Point along range."
   "@returns normalized time used to interpolate values"
   "@ingroup Math")
{
   return mInvLerp(v1, v2, point);
}

DefineEngineFunction( mPi, F32, (),,
    "Return the value of PI (half-circle in radians).\n"
    "@returns The value of PI."
    "@ingroup Math" )
{
   return M_PI_F;
}

DefineEngineFunction( m2Pi, F32, (),,
    "Return the value of 2*PI (full-circle in radians).\n"
    "@returns The value of 2*PI."
    "@ingroup Math" )
{
   return M_2PI_F;
}

DefineEngineFunction( mIsPow2, bool, ( S32 v ),,
    "Returns whether the value is an exact power of two.\n"
    "@param v Input value."
    "@returns Whether the specified value is an exact power of two."
    "@ingroup Math" )
{
   return isPow2( v );
}

DefineEngineFunction( mRandomDir, Point3F, (Point3F axis, F32 angleMin, F32 angleMax),,
   "Returns a randomized direction based on a starting axis and the min/max angles.\n"
   "@param axis Main axis to deviate the direction from."
   "@param angleMin minimum amount of deviation from the axis."
   "@param angleMax maximum amount of deviation from the axis."
   "@returns Randomized direction vector."
   "@ingroup Math")
{
   return MathUtils::randomDir(axis, angleMin, angleMax);
}

DefineEngineFunction( mRandomPointInSphere, Point3F, (F32 radius), ,
   "Returns a randomized point inside a sphere of a given radius.\n"
   "@param radius The radius of the sphere to find a point in."
   "@returns Randomized point inside a sphere."
   "@ingroup Math")
{
   return MathUtils::randomPointInSphere(radius);
}

DefineEngineFunction( mGetAngleBetweenVectors, F32, (VectorF vecA, VectorF vecB), ,
   "Returns angle between two vectors.\n"
   "@param vecA First input vector."
   "@param vecB Second input vector."
   "@returns Angle between both vectors in radians."
   "@ingroup Math")
{
   return MathUtils::getAngleBetweenVectors(vecA, vecB);
}

DefineEngineFunction(mGetSignedAngleBetweenVectors, F32, (VectorF vecA, VectorF vecB, VectorF norm), (VectorF::Zero, VectorF::Zero, VectorF::Zero),
   "Returns signed angle between two vectors, using a normal for orientation.\n"
   "@param vecA First input vector."
   "@param vecB Second input vector."
   "@param norm Normal/Cross Product vector."
   "@returns Angle between both vectors in radians."
   "@ingroup Math")
{
   if (vecA.isZero() || vecB.isZero() || norm.isZero())
   {
      Con::errorf("mGetSignedAngleBetweenVectors - Error! Requires all 3 vectors used to be non-zero!");
      return 0;
   }

   return MathUtils::getSignedAngleBetweenVectors(vecA, vecB, norm);
}

DefineEngineFunction(mBinToDec, S32, (String n),,"convert a binary to decimal")
{
   String num = n;
   int dec_value = 0;

   // Initializing base value to 1, i.e 2^0
   int base = 1;

   int len = num.length();
   for (int i = len - 1; i >= 0; i--) {
      if (num[i] == '1')//pick out our 1s and concatenate
         dec_value += base;
      base = base * 2;//next power of 2
   }

   return dec_value;
}

DefineEngineFunction(mDecToBin, const char*, (S32 n), , "convert decimal to a binary")
{
   String ret;
   while (n > 0) {
      int r = n % 2;//modulus aka remainder of 2. nets you a 0 or a 1
      n /= 2;//next power of 2
      ret = String::ToString("%i",r) + ret;//add to the front of the stack
   }
   return ret.c_str();
}

DefineEngineFunction(mEulDegToAng, AngAxisF, (EulerF euler), , "convert euler to degrees")
{
   return mEulDegToAng(euler);
}

DefineEngineFunction(mAngToEul, EulerF, (AngAxisF angAx), , "convert degrees to euler")
{
   return mAngToEul(angAx);
}
