
#include "stdafx.h"
#include "mathutility.h"

using namespace common;


//-----------------------------------------------------------------------------
// Name: getRandomMinMax()
// Desc: Gets a random number between min/max boundaries
// http://www.codesampler.com/dx9src/dx9src_7.htm
//-----------------------------------------------------------------------------
float common::GetRandomMinMax(float fMin, float fMax)
{
	float fRandNum = (float)rand() / RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
}

//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
// http://www.codesampler.com/dx9src/dx9src_7.htm
//-----------------------------------------------------------------------------
Vector3 common::GetRandomVector(void)
{
	Vector3 vVector;

	// Pick a random Z between -1.0f and 1.0f.
	vVector.z = GetRandomMinMax(-1.0f, 1.0f);

	// Get radius of this circle
	float radius = (float)sqrt(1 - vVector.z * vVector.z);

	// Pick a random point on a circle.
	float t = GetRandomMinMax(-MATH_PI, MATH_PI);

	// Compute matching X and Y for our Z.
	vVector.x = (float)cosf(t) * radius;
	vVector.y = (float)sinf(t) * radius;

	return vVector;
}


// 보간.
void common::lerp(OUT Vector3 &out, const Vector3 &a, const Vector3 &b, float t)
{
	out.x = a.x + (b.x - a.x) * t;
	out.y = a.y + (b.y - a.y) * t;
	out.z = a.z + (b.z - a.z) * t;
}
void common::lerp(OUT Vector2 &out, const Vector2 &a, const Vector2 &b, float t)
{
	out.x = a.x + (b.x - a.x) * t;
	out.y = a.y + (b.y - a.y) * t;
}


// return bezier curve point
// point[0 ~ 3]: bezier point
// t : 0.0 ~ 1.0
void common::bezier(OUT Vector3 &out, const vector<Vector3> &points, const float t)
{
	Vector3 ab, bc, cd, abbc, bccd;
	common::lerp(ab, points[0], points[1], t);
	common::lerp(bc, points[1], points[2], t);
	common::lerp(cd, points[2], points[3], t);
	common::lerp(abbc, ab, bc, t);
	common::lerp(bccd, bc, cd, t);
	common::lerp(out, abbc, bccd, t);
}
void common::bezier(OUT Vector3 &out, const Vector3 points[4], const float t)
{
	Vector3 ab, bc, cd, abbc, bccd;
	common::lerp(ab, points[0], points[1], t);
	common::lerp(bc, points[1], points[2], t);
	common::lerp(cd, points[2], points[3], t);
	common::lerp(abbc, ab, bc, t);
	common::lerp(bccd, bc, cd, t);
	common::lerp(out, abbc, bccd, t);
}
void common::bezier(OUT Vector2 &out, const vector<Vector2> &points, const float t)
{
	Vector2 ab, bc, cd, abbc, bccd;
	common::lerp(ab, points[0], points[1], t);
	common::lerp(bc, points[1], points[2], t);
	common::lerp(cd, points[2], points[3], t);
	common::lerp(abbc, ab, bc, t);
	common::lerp(bccd, bc, cd, t);
	common::lerp(out, abbc, bccd, t);
}

// calc quadratic bezier arc length
// tricky code: calc 50% of bezier arc, and then x2
// points: quadratic bezier line, array[4]
// slice: bezier line slice count to calc arc length
float common::bezierLength(const vector<Vector3>& points, const int slice)
{
	if (slice <= 0) return 0.f;
	if (points.size() < 4) return 0.f;

	const float inc = (1.0f / slice) * 0.5f;
	float a = 0.f;
	float length = 0.f;
	Vector3 prev;
	Vector3 pos;
	for (int i = 0; i < slice + 1; ++i)
	{
		bezier(pos, points, a);
		if (0 != i)
			length += prev.Distance(pos);
		prev = pos;
		a += inc;
	}
	return length * 2.f;
}


// val : 0 ~ 1
template <class T>
inline T lerp(const T _min, const T _max, const T val)
{
	return (1.f - val) * _min + (val * _max);
}

float common::clamp(const float _min, const float _max, const float val)
{
	if (val < _min)
		return _min;
	if (val > _max)
		return _max;
	return val;
}

float common::clamp2(const float val, const float _min, const float _max)
{
	if (val < _min)
		return _min;
	if (val > _max)
		return _max;
	return val;
}

float common::centerRate(const float rate0, const float center, const float val)
{
	return ((val - center) * rate0) + center;
}


// http://www.gisdeveloper.co.kr/?p=89
// 두 직선 AP1,AP2 - BP1,BP2 의 교점을 계산해 리턴한다.
bool common::GetIntersectPoint(const Vector2& AP1, const Vector2& AP2, const Vector2& BP1, const Vector2& BP2, Vector2* IP)
{
	float t;
	float s;
	float under = (BP2.y - BP1.y)*(AP2.x - AP1.x) - (BP2.x - BP1.x)*(AP2.y - AP1.y);
	if (under == 0)
		return false;

	float _t = (BP2.x - BP1.x)*(AP1.y - BP1.y) - (BP2.y - BP1.y)*(AP1.x - BP1.x);
	float _s = (AP2.x - AP1.x)*(AP1.y - BP1.y) - (AP2.y - AP1.y)*(AP1.x - BP1.x);

	t = _t / under;
	s = _s / under;

	if (t<0.0 || t>1.0 || s<0.0 || s>1.0)
		return false;
	if (_t == 0 && _s == 0)
		return false;

	IP->x = AP1.x + t * (AP2.x - AP1.x);
	IP->y = AP1.y + t * (AP2.y - AP1.y);

	return true;
}


// return 0.0 ~ 1.0
float common::Saturate(const float f)
{
	return min(1.f, max(0.f, f));
}


// return -1.0 ~ +1.0
float common::Saturate2(const float f)
{
	return min(1.f, max(-1.f, f));
}



// Line : p0 - p1
// Point : p
// Calc Shortest Length Point - Line
float common::GetShortestLen(const Vector3 &p0, const Vector3 &p1, const Vector3 &p)
{
	const Vector3 center = (p0 + p1) / 2.f;
	const Vector3 dir = (p1 - p0).Normal();
	const float lineLen = (p1 - p0).Length() / 2.f;
	const Vector3 toPoint = p - center;
	float distOnLine = toPoint.DotProduct(dir) / lineLen;
	distOnLine = clamp(-1.f, 1.f, distOnLine) * lineLen;
	const Vector3 pointOnLine = center + dir * distOnLine;
	const float dist = pointOnLine.Distance(p);

	return dist;
}


// Ray : ray
// Point : p
// Calc Shortest Length Point - Line
float common::GetShortestLen(const Ray &ray, const Vector3 &p)
{
	const float a = (p - ray.orig).DotProduct(ray.dir);
	const float b = p.Distance(ray.orig);
	const float len = sqrt(abs(b*b - a*a));
	return len;
}


// return acceleration time to out speed
// v0: enterance speed
// v1: out speed
// a: acceleration
// dist: total moving distance
float common::GetAccelTime(const float v0, const float v1, const float a,
	const float dist)
{
	const float A = a;
	const float B = 2.f * v0;
	const float C = (v0 * v0) / (2.f * a) - (v1 * v1) / (2.f * a) - dist;
	// root's formula
	const float X1 = (-B + sqrt(B * B - (4 * A * C))) / (2.0f * A);
	const float X2 = (-B - sqrt(B * B - (4 * A * C))) / (2.0f * A);
	const bool b1 = (X1 < 0.f);
	const bool b2 = (X2 < 0.f);
	const float t = (b1 && b2)? 0.f : (b1? X2 : X1);

	// check
	//const float vm = v0 + a * t;
	//const float t1 = (vm - v1) / a;
	//const float d1 = 0.5f * a * t * t + v0 * t;
	//const float d2 = 0.5f * -a * t1 * t1 + vm * t1;
	//const float vo = -a * t1 + vm;
	//const float totDist = d1 + d2;
	return t;
}


// return different angle
// https://stackoverflow.com/questions/1878907/how-can-i-find-the-smallest-difference-between-two-angles-around-a-point
// angle1, angle2: radian angle
int common::GetDiffAngle(const float angle1, const float angle2)
{
	const int a = (int)RAD2ANGLE(angle1);
	const int b = (int)RAD2ANGLE(angle2);
	const int an = abs(a - b);
	const int res = abs((an + 180) % 360 - 180);
	return res;
}


// return different angle(radian)
// angle1, angle2: radian angle
float common::GetDiffAngle2(const float angle1, const float angle2)
{
	const float d = abs(angle1 - angle2);
	const float r = (d + MATH_PI) / (MATH_PI * 2.f);
	const float res = abs((d + MATH_PI) - (floor(r) * MATH_PI * 2.f) - MATH_PI);
	return res;
}


// return different angle(radian) with direction (+/-)
// move from -> to
// fromAangle, toAngle: radian angle
float common::GetDiffAngle3(const float fromAngle, const float toAngle)
{
	const float from = SaturateAngle(fromAngle);
	const float to = SaturateAngle(toAngle);
	float d = abs(to - from);

	float res = GetDiffAngle2(toAngle, fromAngle);
	if (from > to)
		res = -res;
	if (d > MATH_PI)
		res = -res;
	return res;
}


// left radian angle
// from: from angle
// to: to angle
// ratio: 0 ~ 1
float common::LerpAngle(const float from, const float to, const float ratio)
{



	return 0.f;

	//const float CS = (1.f - ratio) * cos(from) + ratio * cos(to);
	//const float SN = (1.f - ratio) * sin(from) + ratio * sin(to);
	//return atan2(SN, CS);
}


// return -PI ~ +PI radian angle
// angle: radian angle
float common::SaturateAngle(const float angle)
{
	// saturate -2PI ~ 2PI
	const float r = abs(angle / (MATH_PI * 2.f));
	float a = abs(angle) - (floor(r) * (MATH_PI * 2.f));
	if (angle < 0.f)
		a = -a;

	// saturate -PI ~ PI
	float res = a;
	if (MATH_PI < a)
	{
		res = a - (MATH_PI * 2.f);
	}
	else if (-MATH_PI > a)
	{
		res = a + (MATH_PI * 2.f);
	}
	return res;
}


// return -PI ~ +PI radian angle
// angle: radian angle
double common::SaturateAngle2(const double angle)
{
	// saturate -2PI ~ 2PI
	const double r = abs(angle / (MATH_PI2 * 2.0));
	double a = abs(angle) - (floor(r) * (MATH_PI2 * 2.0));
	if (angle < 0.0)
		a = -a;

	// saturate -PI ~ PI
	double res = a;
	if (MATH_PI2 < a)
	{
		res = a - (MATH_PI2 * 2.0);
	}
	else if (-MATH_PI2 > a)
	{
		res = a + (MATH_PI2 * 2.0);
	}
	return res;
}


// return 0 ~ 2PI radian angle
// angle: radian angle
double common::SaturateAngle2PI(const double angle)
{
	// saturate -2PI ~ 2PI
	const double r = abs(angle / (MATH_PI2 * 2.0));
	double a = abs(angle) - (floor(r) * (MATH_PI2 * 2.0));
	if (angle < 0.0)
		a = -a;

	// saturate 0 ~ 2PI
	double res = a;
	if (a < 0.0)
		res = a + (MATH_PI2 * 2.0);
	return res;
}


// calc tetrahedron volume
// Heron-type formula for the volume of a tetrahedron
// https://en.wikipedia.org/wiki/Tetrahedron
float common::GetTetrahedronVolume(const float U, const float u, const float V
	, const float v, const float W, const float w)
{
	const double X = (w - U + v) * (U + v + w);
	const double Y = (u - V + w) * (V + w + u);
	const double Z = (v - W + u) * (W + u + v);
	const double x = (U - v + w) * (v - w + U);
	const double y = (V - w + u) * (w - u + V);
	const double z = (W - u + v) * (u - v + W);

	const double p = sqrt(x * Y * Z);
	const double q = sqrt(y * Z * X);
	const double r = sqrt(z * X * Y);
	const double s = sqrt(x * y * z);

	const double child = sqrt(
		(-p + q + r + s) * (p - q + r + s) * (p + q - r + s) * (p + q + r - s));
	const double parent = 192.0 * u * v * w;
	const double volume = child / parent;
	return (float)volume;
}


// calc triangle area (Heron's formula)
// https://en.wikipedia.org/wiki/Heron%27s_formula
float common::GetTriangleArea(const float a, const float b, const float c)
{
	const double s = 0.5 * (a + b + c);
	const double area = sqrt(s * (s - a) * (s - b) * (s - c));
	return (float)area;
}


// calc triangle remain edge length
// a,b: edge length
// angle: angle between a,b edge (radian)
// https://mathbang.net/160#gsc.tab=0
float common::GetTriangleEdge(const float a, const float b, const float angle)
{
	const double s = sin(angle);
	const double c = cos(angle);
	const double len = sqrt(s * s + (a - c) * (a - c));
	return (float)len;
}
