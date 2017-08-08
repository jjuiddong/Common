#pragma once

// Direct3D 9 Math Library
#define USE_D3D9_MATH
//#define USE_D3D11_MATH



const float MATH_PI = 3.141592654f;
const float MATH_EPSILON = 0.0005f;//1.0e-5;
const double MATH_EPSILON2 = 0.0000000001f;//1.0e-10;
#define SQR(x)		( (x) * (x) )
#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}
#define FLOAT_EQ(x,v)	(fabs((x)-(v)) < (MATH_EPSILON))
#define ABS( x )	( (x) >= 0 ? (x) : -(x) )
#define ANGLE2RAD(angle) ((MATH_PI*(angle)) * (0.00555555f))
#define RAD2ANGLE(radian) ((radian) * (180.0f/MATH_PI))


#include <math.h>
#include <float.h>
#include <vector>
using std::vector;


#ifdef USE_D3D11_MATH
	#pragma warning(push)
	#pragma warning (disable: 4005) //warning C4005: 'DXGI_ERROR_REMOTE_OUTOFMEMORY': macro redefinition
		#include <d3d11.h>
		#include <dxgitype.h>
	#pragma warning(pop)

	#include <DirectXMath.h>
	#include <DirectXPackedVector.h>
	#include <DirectXCollision.h>

	using namespace DirectX;
#endif // USE_D3D11_MATH


#include "vectortype.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix44.h"
#include "quaternion.h"
#include "transform.h"
#include "triangle.h"
#include "plane.h"
#include "ray.h"
#include "mathutility.h"
