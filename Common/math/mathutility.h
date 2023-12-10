//
// math utility
//
//
#pragma once


namespace common
{

	float GetRandomMinMax(float fMin, float fMax);

	Vector3 GetRandomVector(void);

	void lerp(OUT Vector3 &out, const Vector3 &a, const Vector3 &b, float t);
	void lerp(OUT Vector2& out, const Vector2& a, const Vector2& b, float t);

	void bezier(OUT Vector3& out, const vector<Vector3>& points, const float t);
	void bezier(OUT Vector3& out, const Vector3 points[4], const float t);
	void bezier(OUT Vector2& out, const vector<Vector2>& points, const float t);

	float bezierLength(const vector<Vector3>& points, const int slice);

	// val : 0 ~ 1
	template <class T>
	inline T lerp(const T _min, const T _max, const T val)
	{
		return (1.f - val) * _min + (val * _max);
	}

	float clamp(const float _min, const float _max, const float val);
	float clamp2(const float val, const float _min, const float _max);

	float centerRate(const float rate0, const float center, const float val);

	bool GetIntersectPoint(const Vector2& AP1, const Vector2& AP2
		, const Vector2& BP1, const Vector2& BP2, Vector2* IP);

	float Saturate(const float f);

	float GetShortestLen(const Vector3& p0, const Vector3& p1, const Vector3& p);
	float GetShortestLen(const Ray& ray, const Vector3& p);

	float GetAccelTime(const float v0, const float v1, const float a, const float dist);

	int GetDiffAngle(const float angle1, const float angle2);
	float GetDiffAngle2(const float angle1, const float angle2);
	float GetDiffAngle3(const float toAngle, const float fromAngle);
	float SaturateAngle(const float angle);
	double SaturateAngle2(const double angle);
	double SaturateAngle2PI(const double angle);

}
