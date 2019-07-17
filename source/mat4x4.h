#pragma once

#include "vec4.h"

typedef struct mat4x4
{
	vec4 xAxis;
	vec4 yAxis;
	vec4 zAxis;
	vec4 wAxis;
} mat4x4;

#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

inline void mat4x4_orthoInverse(mat4x4 *mat, mat4x4 *invMat)
{
	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = VecShuffle_0101(mat->xAxis, mat->yAxis); // 00, 01, 10, 11
	__m128 t1 = VecShuffle_2323(mat->xAxis, mat->yAxis); // 02, 03, 12, 13
	invMat->xAxis = VecShuffle(t0, mat->zAxis, 0,2,0,3); // 00, 10, 20, 23(=0)
	invMat->yAxis = VecShuffle(t0, mat->zAxis, 1,3,1,3); // 01, 11, 21, 23(=0)
	invMat->zAxis = VecShuffle(t1, mat->zAxis, 0,2,2,3); // 02, 12, 22, 23(=0)

	// last line
	invMat->wAxis =                           _mm_mul_ps(invMat->xAxis, VecSwizzle1(mat->wAxis, 0));
	invMat->wAxis = _mm_add_ps(invMat->wAxis, _mm_mul_ps(invMat->yAxis, VecSwizzle1(mat->wAxis, 1)));
	invMat->wAxis = _mm_add_ps(invMat->wAxis, _mm_mul_ps(invMat->zAxis, VecSwizzle1(mat->wAxis, 2)));
	invMat->wAxis = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), invMat->wAxis);
}

inline mat4x4 mat4x4_lookAt(vec4 eye, vec4 at, vec4 up)
{
	const vec4 zAxis = vec4_normalize(vec4_sub(eye, at));
	const vec4 xAxis = vec4_normalize(vec4_cross(up, zAxis));
	const vec4 yAxis = vec4_cross(zAxis, xAxis);
		
	mat4x4 worldMatrix;
	worldMatrix.xAxis = xAxis;
	worldMatrix.yAxis = yAxis;
	worldMatrix.zAxis = zAxis;
	worldMatrix.wAxis = eye;
		
	mat4x4 res;
	mat4x4_orthoInverse(&worldMatrix, &res);

	return res;
}

inline mat4x4 mat4x4_frustum(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	float A = (right + left) / (right - left);
	float B = (top + bottom) / (top - bottom);
	float C = -(farZ + nearZ) / (farZ - nearZ);
	float D = -(2.0f * farZ * nearZ) / (farZ - nearZ);

	mat4x4 res;
	res.xAxis = vec4_init(2.0f * nearZ / (right - left), 0.0f, 0.0f, 0.0f);
	res.yAxis = vec4_init(0.0f, 2.0f * nearZ / (top - bottom), 0.0f, 0.0f);		
	res.zAxis = vec4_init(A, B, C, -1.0f);
	res.wAxis = vec4_init(0.0f, 0.0f, D, 0.0);

	return res;
}

inline void mat4x4_translate(vec4 translate, mat4x4 *restrict res)
{
	res->xAxis = vec4_init(1.0f, 0.0f, 0.0f, 0.0f);
	res->yAxis = vec4_init(0.0f, 1.0f, 0.0f, 0.0f);
	res->zAxis = vec4_init(0.0f, 0.0f, 1.0f, 0.0f);
	res->wAxis = translate;
}

inline void mat4x4_rotate(float angle, vec4 axis, mat4x4 *restrict res)
{
	__attribute__ ((aligned(16))) float axisMem[4] = {};
	vec4_store(axisMem, axis);

	float ax = axisMem[0];
	float ay = axisMem[1];
	float az = axisMem[2];
	float xx = ax * ax;
	float yy = ay * ay;
	float zz = az * az;
	
	float sina = sinf(angle);
	float cosa = cosf(angle);
		
	float oneMinusC = 1.0f - cosa;
		
	float m00 = cosa + xx * oneMinusC;
	float m01 = ax * ay * oneMinusC + az * sina;
	float m02 = ax * az * oneMinusC - ay * sina;
		
	float m10 = ax * ay * oneMinusC - az * sina;
	float m11 = cosa + yy * oneMinusC;
	float m12 = ay * az * oneMinusC + ax * sina;
		
	float m20 = ax * az * oneMinusC + ay * sina;
	float m21 = ay * az * oneMinusC - ax * sina;
	float m22 = cosa + zz * oneMinusC;

	res->xAxis = vec4_init(m00, m01, m02, 0.0f);
	res->yAxis = vec4_init(m10, m11, m12, 0.0f);
	res->zAxis = vec4_init(m20, m21, m22, 0.0f);
	res->wAxis = vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
}

/*
mat4x4 mat4x4_transform(mat4x4 m, vec4 vec)
{
	vec4 res, acc;
	float x = right.X();
			float y = right.Y();
			float z = right.Z();
			float w = right.W();
	vec4 acc = m.wAxis * w;
			acc = acc + m_zAxis * z;
			acc = acc + m_yAxis * y;
			res = acc + m_xAxis * x;

			return res;

}
*/

inline mat4x4 mat4x4_mul(float *restrict A, float *restrict B, float *restrict C)
{
	mat4x4 res;

	//res.m_xAxis = m1 * m2.xAxis;
	//res.m_yAxis = m1 * m2.yAxis;
	//res.m_zAxis = m1 * m2.zAxis;
	//res.m_wAxis = m1 * m2.wAxis;

    __m128 row1 = _mm_load_ps(&B[0]);
    __m128 row2 = _mm_load_ps(&B[4]);
    __m128 row3 = _mm_load_ps(&B[8]);
    __m128 row4 = _mm_load_ps(&B[12]);
    for(int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(A[4*i + 0]);
        __m128 brod2 = _mm_set1_ps(A[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(A[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(A[4*i + 3]);
        __m128 row = _mm_add_ps(
                        _mm_add_ps(
                        _mm_mul_ps(brod1, row1),
                        _mm_mul_ps(brod2, row2)),
                     _mm_add_ps(
                        _mm_mul_ps(brod3, row3),
                        _mm_mul_ps(brod4, row4)));
        _mm_store_ps(&C[4*i], row);
    }
	return res;		
}
