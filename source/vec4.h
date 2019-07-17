#pragma once
//#ifndef VEC4_H
//#define VEC4_H

//#include <x86intrin.h>
#include <smmintrin.h>

typedef __m128 vec4;

inline vec4 vec4_init(float x, float y, float z, float w)
{
	return _mm_set_ps(w, z, y, x);
}

inline vec4 vec4_load(float *restrict address)
{
	return _mm_load_ps(address);
}

inline void vec4_store(float *restrict address, vec4 vec)
{
	_mm_store_ps(address, vec);
}

inline vec4 vec4_add(vec4 v1, vec4 v2)
{
	return _mm_add_ps(v1, v2);	
}

inline vec4 vec4_sub(vec4 v1, vec4 v2)
{
	return _mm_sub_ps(v1, v2);	
}

inline vec4 vec4_scale(vec4 v1, vec4 v2)
{
	return _mm_sub_ps(v1, v2);	
}

inline vec4 vec4_dot(vec4 v1, vec4 v2)
{
	const int imm8 = 0;
	return _mm_dp_ps(v1, v2, 0xff);
}

inline vec4 vec4_cross(vec4 a, vec4 b)
{
	return _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))), 
		   			  _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
}

inline vec4 vec4_normalize(vec4 v)
{
	// 0x7F = 0111 1111 ~ means we don't want the w-component multiplied
	// and the result written to all 4 components
	vec4 dp = _mm_dp_ps(v, v, 0x7f); 
		
	// compute rsqrt of the dot product
	dp = _mm_rsqrt_ps(dp);

	// vec * rsqrt(dot(vec, vec))
	return _mm_mul_ps(v, dp);	
}

//#endif
