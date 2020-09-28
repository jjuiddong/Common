//
// 2018-05-03, jjuiddong
// vector3 template class
//
#pragma once

namespace common
{
	template<class T>
	struct Vector2Comm
	{
		T x, y;

		Vector2Comm<T>() : x(0), y(0) {}
		Vector2Comm<T>(T x0, T y0) : x(x0), y(y0) {}

		bool IsEmpty() const;
		float Length() const;
		float Length2() const;
		float LengthRoughly(const Vector2Comm<T> &rhs) const;
		float Distance(const Vector2Comm<T> &rhs) const;
		Vector2Comm<T> Normal() const;
		void Normalize();
		Vector2Comm<T> Interpolate(const Vector2Comm<T> &v, const float alpha) const;
		Vector2Comm<T> Minimum(const Vector2Comm<T> &rhs) const;
		Vector2Comm<T> Maximum(const Vector2Comm<T> &rhs) const;

		T DotProduct(const Vector2Comm<T>& v) const;
		Vector2Comm<T> CrossProduct(const Vector2Comm<T>& v) const;

		Vector2Comm<T> operator + () const;
		Vector2Comm<T> operator - () const;
		Vector2Comm<T> operator + (const Vector2Comm<T>& rhs) const;
		Vector2Comm<T> operator - (const Vector2Comm<T>& rhs) const;
		Vector2Comm<T>& operator += (const Vector2Comm<T>& rhs);
		Vector2Comm<T>& operator -= (const Vector2Comm<T>& rhs);
		Vector2Comm<T>& operator *= (const Vector2Comm<T>& rhs);
		Vector2Comm<T>& operator /= (const Vector2Comm<T>& rhs);

		bool operator < (const Vector2Comm<T> &rhs) const; // for sorting
		bool operator == (const Vector2Comm<T> &rhs) const;
		bool operator != (const Vector2Comm<T> &rhs) const;
		Vector2Comm<T> operator * (const Vector2Comm<T> &v) const;

		template <class Type>
		Vector2Comm<T> operator * (Type t) const {
			return Vector2Comm<T>(x*(T)t, y*(T)t);
		}

		template <class Type>
		Vector2Comm<T> operator / (Type t) const {
			return Vector2Comm<T>(x / (T)t, y / (T)t);
		}

		template <class Type>
		Vector2Comm<T>& operator *= (Type t) {
			*this = Vector2Comm<T>(x*(T)t, y*(T)t);
			return *this;
		}

		template <class Type>
		Vector2Comm<T>& operator /= (Type t) {
			*this = Vector2Comm<T>(x / (T)t, y / (T)t);
			return *this;
		}

	};


	template<class T>
	bool Vector2Comm<T>::IsEmpty() const {
		return (x == 0) && (y == 0);
	}

	template<class T>
	float Vector2Comm<T>::Length() const {
		return (float)sqrt(x * x + y * y);
	}

	template<class T>
	float Vector2Comm<T>::Length2() const {
		return x * x + y * y;
	}

	template<class T>
	float Vector2Comm<T>::LengthRoughly(const Vector2Comm<T> &rhs) const {
		Vector2Comm<T> v = *this - rhs;
		return v.x*v.x + v.y*v.y;
	}

	template<class T>
	float Vector2Comm<T>::Distance(const Vector2Comm<T> &rhs) const {
		Vector2Comm<T> v = *this - rhs;
		return v.Length();
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::Normal() const {
		const float len = Length();
		if (len < 0.001f)
			return Vector2Comm<T>(0, 0);
		return *this / len;
	}

	template<class T>
	void Vector2Comm<T>::Normalize() {
		*this = Normal();
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::Interpolate(const Vector2Comm<T> &v, const float alpha) const {
		return Vector2Comm<T>(x + (alpha * (v.x - x)),
			y + (alpha * (v.y - y))
			);
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::Minimum(const Vector2Comm<T> &rhs) const {
		Vector2Comm<T> v;
		v.x = min(x, rhs.x);
		v.y = min(y, rhs.y);
		return v;
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::Maximum(const Vector2Comm<T> &rhs) const {
		Vector2Comm<T> v;
		v.x = max(x, rhs.x);
		v.y = max(y, rhs.y);
		return v;
	}

	template<class T>
	T Vector2Comm<T>::DotProduct(const Vector2Comm<T>& v) const
	{
		return x * v.x + y * v.y;
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::CrossProduct(const Vector2Comm<T>& v) const
	{
		assert(0);
		return {};
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::operator + () const {
		return *this;
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::operator - () const {
		return Vector2Comm<T>(-x, -y);
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::operator + (const Vector2Comm<T>& rhs) const {
		return Vector2Comm<T>(x + rhs.x, y + rhs.y);
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::operator - (const Vector2Comm<T>& rhs) const {
		return Vector2Comm<T>(x - rhs.x, y - rhs.y);
	}

	template<class T>
	Vector2Comm<T>& Vector2Comm<T>::operator += (const Vector2Comm<T>& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	template<class T>
	Vector2Comm<T>& Vector2Comm<T>::operator -= (const Vector2Comm<T>& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	template<class T>
	Vector2Comm<T>& Vector2Comm<T>::operator *= (const Vector2Comm<T>& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	template<class T>
	Vector2Comm<T>& Vector2Comm<T>::operator /= (const Vector2Comm<T>& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}

	template<class T>
	bool Vector2Comm<T>::operator < (const Vector2Comm<T> &rhs) const
	{
		return (x < rhs.x) || ((x == rhs.x) && (y < rhs.y));
	}

	template<class T>
	bool Vector2Comm<T>::operator == (const Vector2Comm<T> &rhs) const
	{
		return (abs(x - rhs.x) < MATH_EPSILON) &&
			(abs(y - rhs.y) < MATH_EPSILON);
	}

	template<class T>
	bool Vector2Comm<T>::operator != (const Vector2Comm<T> &rhs) const
	{
		return !(operator==(rhs));
	}

	template<class T>
	Vector2Comm<T> Vector2Comm<T>::operator * (const Vector2Comm<T> &v) const {
		return Vector2Comm<T>(x*v.x, y*v.y);
	}


	typedef Vector2Comm<int> Vector2i;
	typedef Vector2Comm<float> Vector2f;
	typedef Vector2Comm<double> Vector2d;
}
