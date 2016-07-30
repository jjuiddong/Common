//
// Spline 곡선 계산을 하는 객체.
// Catmull Rom Spline 곡선을 계산한다.
//
#pragma once


class cSpline
{
public:
	cSpline();
	virtual ~cSpline();

	void Init(const bool enable, const int samplingRate=1, const int interpolationRate=1);
	void AddPoint(const Vector2 &pos);
	bool AddPoint(const float deltaSeconds, const Vector2 &pos);
	Vector2 GetInterpolation(const float t);
	bool GetInterpolations(const float s, const float t, vector<Vector2> &out);
	bool IsEnable() const;
	int GetStoreCount() const;


public:
	bool m_enable;
	bool m_isReadEnable;
	int m_samplingRate;
	int m_interplationRate;


protected:
	int m_storeCount;
	float m_samplingTime;
	float m_interpoationStep;
	float m_incTime;

	vector<Vector2> m_vec; // 4개 포인트만 저장한다.
};


inline bool cSpline::IsEnable() const { return m_enable;  }
inline int cSpline::GetStoreCount() const { return m_storeCount; }