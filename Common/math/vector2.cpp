
#include "stdafx.h"
#include "vector2.h"

using namespace common;


float Vector2::Length() const
{
	return sqrt(x*x + y*y);
}


Vector2 Vector2::Normal() const
{
	const float len = Length();
	if (len < 0.001f)
		return Vector2(0, 0);
	return *this / len;
}


void Vector2::Normalize()
{
	*this = Normal();
}


Vector2 Vector2::operator + () const
{
	return *this;
}


Vector2 Vector2::operator - () const
{
	return Vector2(-x, -y);
}


Vector2 Vector2::operator + ( const Vector2& rhs ) const
{
	return Vector2(x+rhs.x, y+rhs.y);
}


Vector2 Vector2::operator - ( const Vector2& rhs ) const
{
	return Vector2(x-rhs.x, y-rhs.y);
}


Vector2& Vector2::operator += ( const Vector2& rhs )
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}


Vector2& Vector2::operator -= ( const Vector2& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}


Vector2& Vector2::operator *= ( const Vector2& rhs )
{
	x *= rhs.x;
	y *= rhs.y;
	return *this;
}


Vector2& Vector2::operator /= ( const Vector2& rhs )
{
	x /= rhs.x;
	y /= rhs.y;
	return *this;
}
