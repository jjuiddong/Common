#pragma once

namespace common
{

	struct Vector2
	{
		float x, y;

		Vector2():x(0.f), y(0.f) {}
		Vector2(float x0, float y0) : x(x0), y(y0) {}

		inline bool IsEmpty() const;
		inline bool IsEqual(const Vector2& rhs, const float epsilon) const;
		float Length() const;
		float LengthRoughly(const Vector2& rhs) const;
		Vector2 Normal() const;
		float Distance(const Vector2 &rhs) const;
		void Normalize();

		Vector2 operator + () const;
		Vector2 operator - () const;
		Vector2 operator + ( const Vector2& rhs ) const;
		Vector2 operator - ( const Vector2& rhs ) const;
		Vector2& operator += ( const Vector2& rhs );
		Vector2& operator -= ( const Vector2& rhs );
		Vector2& operator *= ( const Vector2& rhs );
		Vector2& operator /= ( const Vector2& rhs );

		float DotProduct(const Vector2& v) const
		{
			return x * v.x + y * v.y;
		}

		bool operator == (const Vector2& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y);
		}

		bool operator != (const Vector2& rhs) const
		{
			return (x != rhs.x) || (y != rhs.y);
		}

		template <class T>
		Vector2 operator * ( T t ) const {
			return Vector2(x*t, y*t);
		}

		template <class T>
		Vector2 operator / ( T t ) const {
			return Vector2(x/t, y/t);
		}

		template <class T>
		Vector2& operator *= ( T t ) {
			*this = Vector2(x*t, y*t);
			return *this;
		}

		template <class T>
		Vector2& operator /= ( T t ) {
			*this = Vector2(x/t, y/t);
			return *this;
		}

	};


	inline bool Vector2::IsEmpty() const
	{
		return (x == 0) && (y == 0);
	}

	inline bool Vector2::IsEqual(const Vector2& rhs, const float epsilon) const
	{
		return Distance(rhs) < epsilon;
	}

}
