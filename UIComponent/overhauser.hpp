#ifndef overhauserHPP
#define overhauserHPP

//#include "vec3.hpp"
//#include <vector>

class CRSpline
{
public:

    // Constructors and destructor
    CRSpline();
    CRSpline(const CRSpline&);
    ~CRSpline();

    // Operations
	void AddSplinePoint(const Vector2& v);
	Vector2 GetInterpolatedSplinePoint(float t);   // t = 0...1; 0=vp[0] ... 1=vp[max]
	int GetNumPoints();
	Vector2& GetNthPoint(int n);

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
	static Vector2 Eq(float t, const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4);

private:
	std::vector<Vector2> vp;
    float delta_t;
};

#endif
