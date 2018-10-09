//
// 2018-05-03, jjuiddong
// vector3 template class
//
#pragma once

namespace common
{
	struct Vector4;
	struct Matrix44;
	struct Transform;
	struct Quaternion;

	template<class T>
	struct Vector3Comm
	{
		T x, y, z;

		Vector3Comm<T>() : x(0), y(0), z(0) {}
		Vector3Comm<T>(T x0, T y0, T z0) : x(x0), y(y0), z(z0) {}
		Vector3Comm<T>(const Vector4 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

		bool IsEmpty() const;
		float Length() const;
		float Length2() const;
		float LengthRoughly(const Vector3Comm<T> &rhs) const;
		float Distance(const Vector3Comm<T> &rhs) const;
		Vector3Comm<T> Normal() const;
		void Normalize();
		Vector3Comm<T> MultiplyNormal(const Matrix44& rhs) const;
		Vector3Comm<T> MultiplyNormal2(const Matrix44& rhs) const;
		Vector3Comm<T> Interpolate(const Vector3Comm<T> &v, const float alpha) const;
		Vector3Comm<T> Minimum(const Vector3Comm<T> &rhs) const;
		Vector3Comm<T> Maximum(const Vector3Comm<T> &rhs) const;

#ifdef USE_D3D11_MATH
		XMVECTOR GetVectorXM() const;
		Vector3Comm<T> operator * (const XMMATRIX &m) const;
#endif

		T DotProduct(const Vector3Comm<T>& v) const;
		Vector3Comm<T> CrossProduct(const Vector3Comm<T>& v) const;

		Vector3Comm<T> operator + () const;
		Vector3Comm<T> operator - () const;
		Vector3Comm<T> operator + (const Vector3Comm<T>& rhs) const;
		Vector3Comm<T> operator - (const Vector3Comm<T>& rhs) const;
		Vector3Comm<T>& operator += (const Vector3Comm<T>& rhs);
		Vector3Comm<T>& operator -= (const Vector3Comm<T>& rhs);
		Vector3Comm<T>& operator *= (const Vector3Comm<T>& rhs);
		Vector3Comm<T>& operator /= (const Vector3Comm<T>& rhs);

		Vector3Comm<T> operator * (const Matrix44& rhs) const;
		Vector3Comm<T> operator * (const Quaternion& rhs) const;
		Vector3Comm<T>& operator *= (const Matrix44& rhs);
		Vector3Comm<T> operator * (const Transform& rhs) const;
		Vector3Comm<T>& operator *= (const Transform& rhs);

		bool operator == (const Vector3Comm<T> &rhs) const;
		bool operator != (const Vector3Comm<T> &rhs) const;
		Vector3Comm<T> operator * (const Vector3Comm<T> &v) const;

		template <class Type>
		Vector3Comm<T> operator * (Type t) const {
			return Vector3Comm<T>(x*(T)t, y*(T)t, z*(T)t);
		}

		template <class Type>
		Vector3Comm<T> operator / (Type t) const {
			return Vector3Comm<T>(x / (T)t, y / (T)t, z / (T)t);
		}

		template <class Type>
		Vector3Comm<T>& operator *= (Type t) {
			*this = Vector3Comm<T>(x*(T)t, y*(T)t, z*(T)t);
			return *this;
		}

		template <class Type>
		Vector3Comm<T>& operator /= (Type t) {
			*this = Vector3Comm<T>(x / (T)t, y / (T)t, z / (T)t);
			return *this;
		}


		//static Vector3 Min;
		//static Vector3 Max;
		//static Vector3 Up;
		//static Vector3 Right;
		//static Vector3 Forward;
	};


	template<class T>
	bool Vector3Comm<T>::IsEmpty() const {
		return (x == 0) && (y == 0) && (z == 0);
	}

	template<class T>
	float Vector3Comm<T>::Length() const {
		return sqrt(x*x + y * y + z * z);
	}

	template<class T>
	float Vector3Comm<T>::Length2() const {
		return x * x + y * y + z * z;
	}

	template<class T>
	float Vector3Comm<T>::LengthRoughly(const Vector3Comm<T> &rhs) const {
		Vector3Comm<T> v = *this - rhs;
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}

	template<class T>
	float Vector3Comm<T>::Distance(const Vector3Comm<T> &rhs) const {
		Vector3Comm<T> v = *this - rhs;
		return v.Length();
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::Normal() const {
		const float len = Length();
		if (len < 0.001f)
			return Vector3Comm<T>(0, 0, 0);
		return *this / len;
	}

	template<class T>
	void Vector3Comm<T>::Normalize() {
		*this = Normal();
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::MultiplyNormal(const Matrix44& rhs) const {
		Vector3Comm<T> v;
		v.x = x * rhs._11 + y * rhs._21 + z * rhs._31;
		v.y = x * rhs._12 + y * rhs._22 + z * rhs._32;
		v.z = x * rhs._13 + y * rhs._23 + z * rhs._33;
		v.Normalize();
		return v;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::MultiplyNormal2(const Matrix44& rhs) const {
		Vector3Comm<T> v;
		v.x = x * rhs._11 + y * rhs._21 + z * rhs._31;
		v.y = x * rhs._12 + y * rhs._22 + z * rhs._32;
		v.z = x * rhs._13 + y * rhs._23 + z * rhs._33;
		return v;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::Interpolate(const Vector3Comm<T> &v, const float alpha) const {
		return Vector3Comm<T>(x + (alpha * (v.x - x)),
			y + (alpha * (v.y - y)),
			z + (alpha * (v.z - z)));
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::Minimum(const Vector3Comm<T> &rhs) const {
		Vector3Comm<T> v;
		v.x = min(x, rhs.x);
		v.y = min(y, rhs.y);
		v.z = min(z, rhs.z);
		return v;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::Maximum(const Vector3Comm<T> &rhs) const {
		Vector3Comm<T> v;
		v.x = max(x, rhs.x);
		v.y = max(y, rhs.y);
		v.z = max(z, rhs.z);
		return v;
	}

#ifdef USE_D3D11_MATH
	template<class T>
	XMVECTOR Vector3Comm<T>::GetVectorXM() const {
		return XMLoadFloat3((XMFLOAT3*)this);
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator * (const XMMATRIX &m) const {
		XMVECTOR v = XMLoadFloat3((XMFLOAT3*)this);
		v = XMVector3Transform(v, m);
		Vector3Comm<T> ret;
		XMStoreFloat3((XMFLOAT3*)&ret, v);
		return ret;
	}
#endif

	template<class T>
	T Vector3Comm<T>::DotProduct(const Vector3Comm<T>& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::CrossProduct(const Vector3Comm<T>& v) const
	{
		return Vector3Comm<T>(
			(y * v.z) - (z * v.y),
			(z * v.x) - (x * v.z),
			(x * v.y) - (y * v.x));
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator + () const {
		return *this;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator - () const {
		return Vector3(-x, -y, -z);
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator + (const Vector3Comm<T>& rhs) const {
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator - (const Vector3Comm<T>& rhs) const {
		return Vector3Comm<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	template<class T>
	Vector3Comm<T>& Vector3Comm<T>::operator += (const Vector3Comm<T>& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	template<class T>
	Vector3Comm<T>& Vector3Comm<T>::operator -= (const Vector3Comm<T>& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	template<class T>
	Vector3Comm<T>& Vector3Comm<T>::operator *= (const Vector3Comm<T>& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	template<class T>
	Vector3Comm<T>& Vector3Comm<T>::operator /= (const Vector3Comm<T>& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator * (const Matrix44& rhs) const {
		float	RHW = 1.0f / (x*rhs._14 + y * rhs._24 + z * rhs._34 + rhs._44);
		if (RHW >= FLT_MAX)
			return Vector3Comm<T>(0, 0, 0);

		Vector3Comm<T> v;
		v.x = (x*rhs._11 + y * rhs._21 + z * rhs._31 + rhs._41) * RHW;
		v.y = (x*rhs._12 + y * rhs._22 + z * rhs._32 + rhs._42) * RHW;
		v.z = (x*rhs._13 + y * rhs._23 + z * rhs._33 + rhs._43) * RHW;
		return v;
	}

	template<class T>
	Vector3Comm<T>& Vector3Comm<T>::operator *= (const Matrix44& rhs) {
		float	RHW = 1.0f / (x * rhs._14 + y * rhs._24 + z * rhs._34 + rhs._44);
		if (RHW >= FLT_MAX)
		{
			*this = Vector3Comm<T>(0, 0, 0);
			return *this;
		}

		Vector3Comm<T> v;
		v.x = (x * rhs._11 + y * rhs._21 + z * rhs._31 + rhs._41) * RHW;
		v.y = (x * rhs._12 + y * rhs._22 + z * rhs._32 + rhs._42) * RHW;
		v.z = (x * rhs._13 + y * rhs._23 + z * rhs._33 + rhs._43) * RHW;
		*this = v;
		return *this;
	}

	//https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator * (const Quaternion& rhs) const
	{
		// Extract the vector part of the quaternion
		Vector3Comm<T> u(rhs.x, rhs.y, rhs.z);

		// Extract the scalar part of the quaternion
		const float s = rhs.w;

		// Do the math
		return u * 2.0f * u.DotProduct(*this)
			+ *this * (s*s - u.DotProduct(u))
			+ u.CrossProduct(*this) * 2.0f * s;
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator * (const Transform& rhs) const {
		return operator*(rhs.GetMatrix());
	}

	template<class T>
	Vector3Comm<T>& Vector3Comm<T>::operator *= (const Transform& rhs) {
		return operator*=(rhs.GetMatrix());
	}

	template<class T>
	bool Vector3Comm<T>::operator == (const Vector3Comm<T> &rhs) const
	{
		return (abs(x - rhs.x) < MATH_EPSILON) &&
			(abs(y - rhs.y) < MATH_EPSILON) &&
			(abs(z - rhs.z) < MATH_EPSILON);
	}

	template<class T>
	bool Vector3Comm<T>::operator != (const Vector3Comm<T> &rhs) const
	{
		return !(operator==(rhs));
	}

	template<class T>
	Vector3Comm<T> Vector3Comm<T>::operator * (const Vector3Comm<T> &v) const {
		return Vector3Comm<T>(x*v.x, y*v.y, z*v.z);
	}


	typedef Vector3Comm<int> Vector3i;
	typedef Vector3Comm<float> Vector3f;
	typedef Vector3Comm<double> Vector3d;
}
