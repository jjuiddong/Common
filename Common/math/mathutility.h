#pragma once


namespace common
{

	//-----------------------------------------------------------------------------
	// Name: getRandomMinMax()
	// Desc: Gets a random number between min/max boundaries
	// http://www.codesampler.com/dx9src/dx9src_7.htm
	//-----------------------------------------------------------------------------
	inline float GetRandomMinMax(float fMin, float fMax)
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
	inline Vector3 GetRandomVector(void)
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
	inline void lerp(OUT Vector3 &out, const Vector3 &a, const Vector3 &b, float t)
	{
		out.x = a.x + (b.x - a.x) * t;
		out.y = a.y + (b.y - a.y) * t;
		out.z = a.z + (b.z - a.z) * t;
	}
	inline void lerp(OUT Vector2 &out, const Vector2 &a, const Vector2 &b, float t)
	{
		out.x = a.x + (b.x - a.x) * t;
		out.y = a.y + (b.y - a.y) * t;
	}


	// 베지어 곡선을 리턴한다.
	// point[ 0 ~ 3]
	// t : 0 ~ 1
	inline void bezier(OUT Vector3 &out, const vector<Vector3> &points, const float t)
	{
		Vector3 ab, bc, cd, abbc, bccd;
		common::lerp(ab, points[0], points[1], t);
		common::lerp(bc, points[1], points[2], t);
		common::lerp(cd, points[2], points[3], t);
		common::lerp(abbc, ab, bc, t);
		common::lerp(bccd, bc, cd, t);
		common::lerp(out, abbc, bccd, t);
	}
	inline void bezier(OUT Vector3 &out, const Vector3 points[4], const float t)
	{
		Vector3 ab, bc, cd, abbc, bccd;
		common::lerp(ab, points[0], points[1], t);
		common::lerp(bc, points[1], points[2], t);
		common::lerp(cd, points[2], points[3], t);
		common::lerp(abbc, ab, bc, t);
		common::lerp(bccd, bc, cd, t);
		common::lerp(out, abbc, bccd, t);
	}
	inline void bezier(OUT Vector2 &out, const vector<Vector2> &points, const float t)
	{
		Vector2 ab, bc, cd, abbc, bccd;
		common::lerp(ab, points[0], points[1], t);
		common::lerp(bc, points[1], points[2], t);
		common::lerp(cd, points[2], points[3], t);
		common::lerp(abbc, ab, bc, t);
		common::lerp(bccd, bc, cd, t);
		common::lerp(out, abbc, bccd, t);
	}

	// val : 0 ~ 1
	template <class T>
	inline T lerp(const T _min, const T _max, const T val)
	{
		return (1.f - val) * _min + (val * _max);
	}

	inline float clamp(const float _min, const float _max, const float val)
	{
		if (val < _min)
			return _min;
		if (val > _max)
			return _max;
		return val;
	}

	inline float clamp2(const float val, const float _min, const float _max)
	{
		if (val < _min)
			return _min;
		if (val > _max)
			return _max;
		return val;
	}

	inline float centerRate(const float rate0, const float center, const float val)
	{
		return ((val - center) * rate0) + center;
	}


	// http://www.gisdeveloper.co.kr/?p=89
	// 두 직선 AP1,AP2 - BP1,BP2 의 교점을 계산해 리턴한다.
	inline bool GetIntersectPoint(const Vector2& AP1, const Vector2& AP2, const Vector2& BP1, const Vector2& BP2, Vector2* IP)
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

	// 0 ~ +1
	inline float Saturate(const float f)
	{
		return min(1.f, max(0.f, f));
	}


	// Line : p0 - p1
	// Point : p
	// Calc Shortest Length Point - Line
	inline float GetShortestLen(const Vector3 &p0, const Vector3 &p1, const Vector3 &p)
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
	inline float GetShortestLen(const Ray &ray, const Vector3 &p)
	{
		const float a = (p - ray.orig).DotProduct(ray.dir);
		const float b = p.Distance(ray.orig);
		const float len = sqrt(abs(b*b - a*a));
		return len;
	}

}