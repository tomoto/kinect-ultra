#ifndef _VEC_H_
#define _VEC_H_

#include "common.h"
#include "math.h"

/**
 * XnVector3D + vector manipulation methods.
 *
 * Not everything is covered. I'll add features on-demand basis.
 * 
 * @ja
 * XnVector3Dにベクタ演算機能を追加したもの。
 * オンデマンドに機能追加していく。
 *
 * 基本的に結果は戻り値で新しいオブジェクトとして返す。
 * Mで終わるのはthisをmutateする。
 */
class XV3 : public XnVector3D {
public:
	// ctors
	XV3() { X = Y = Z = 0; }
	XV3(float x, float y, float z) { X=x, Y=y, Z=z; }
	XV3(float* v) { X=v[0], Y=v[1], Z=v[2]; }
	XV3(const XnVector3D& v) { X=v.X, Y=v.Y, Z=v.Z; }

	// object lifecycle

	XV3& assign(float x, float y, float z) { X=x, Y=y, Z=z; return *this; }
	XV3& assign(float* v) { X=v[0], Y=v[1], Z=v[2]; return *this; }
	XV3& assign(const XnVector3D& v) { X=v.X, Y=v.Y, Z=v.Z; return *this; }
	XV3& operator=(const XnVector3D& v) { assign(v); return *this; }

	// add, sub, mul, div

	XV3& operator+=(const XnVector3D& v) { X+=v.X, Y+=v.Y, Z+=v.Z; return *this; }
	XV3 operator+(const XnVector3D& v) const { return XV3(X+v.X, Y+v.Y, Z+v.Z); }

	XV3& operator-=(const XnVector3D& v) { X-=v.X, Y-=v.Y, Z-=v.Z; return *this; }
	XV3 operator-(const XnVector3D& v) const { return XV3(X-v.X, Y-v.Y, Z-v.Z); }

	XV3& operator*=(float a) { X*=a, Y*=a, Z*=a; return *this; }
	XV3 operator*(float a) const { return XV3(X*a, Y*a, Z*a); }

	XV3& operator/=(float a) { X/=a, Y/=a, Z/=a; return *this; }
	XV3 operator/(float a) const { return XV3(X/a, Y/a, Z/a); }

	XV3 operator-() const { return XV3(-X, -Y, -Z); }

	// products

	float dot(const XnVector3D& v) const { return X*v.X + Y*v.Y + Z*v.Z; }

	float dotNormalized(const XV3& v) const { return dot(v) / magnitude() / v.magnitude(); }

	XV3& crossM(const XnVector3D& v) { assign(Y*v.Z-Z*v.Y, Z*v.X-X*v.Z, X*v.Y-Y*v.X); return *this; }
	XV3 cross(const XnVector3D& v) const { return XV3(Y*v.Z-Z*v.Y, Z*v.X-X*v.Z, X*v.Y-Y*v.X); }

	// magnitudes

	/** squared magnitude */
	float magnitude2() const { return X*X + Y*Y + Z*Z; }

	float magnitude() const { return sqrt(magnitude2()); }

	/** squared distance */
	float distance2(const XnVector3D& v) const { return (*this - v).magnitude2(); }

	float distance(const XnVector3D& v) const { return sqrt(distance2(v)); }

	// normalizations

	XV3& normalizeM()
	{
		float m = magnitude();
		return assign(X/m, Y/m, Z/m);
	}

	XV3 normalize() const
	{
		float m = magnitude();
		return XV3(X/m, Y/m, Z/m);
	}

	// other derivables

	XV3& interpolateM(const XnVector3D& v, float alpha = 0.5f)
	{
		return assign(::interpolate(X, v.X, alpha), ::interpolate(Y, v.Y, alpha), ::interpolate(Z, v.Z, alpha));
	}

	XV3 interpolate(const XnVector3D& v, float alpha = 0.5f) const
	{
		return XV3(::interpolate(X, v.X, alpha), ::interpolate(Y, v.Y, alpha), ::interpolate(Z, v.Z, alpha));
	}
};

/**
 * Convenient wrapper to convert XV3 to M3DVector3f for GLTools.
 *
 * e.g.
 * XV3 v;
 * GLFrame frame;
 * frame.SetForwardVector(XV3toM3D(v));
 */
class XV3toM3D
{
private:
	M3DVector3f m3d;

public:
	XV3toM3D(const XV3& v) {
		m3d[0]=v.X, m3d[1]=v.Y, m3d[2]=v.Z;
	}
	
	operator float*() {
		return m3d;
	}

	operator const float*() const {
		return m3d;
	}
};

#endif
