//
// 2017-04-20, jjuiddong
// Rect Class
//
#pragma once


namespace common
{
	
	template <class T>
	struct sRect1
	{
		static sRect1 Rect(T x, T y, T width, T height) {
			return sRect1<T>(x, y, x + width, y + height);
		}

		sRect1() : left(0), top(0), right(0), bottom(0) {
		}

		sRect1(T left, T top, T right, T bottom) {
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}

		bool IsIn(const T x, const T y) const {
			return (left <= x)
				&& (right >= x)
				&& (top <= y)
				&& (bottom >= y);
		}

		bool IsContain(const sRect1 &rect) const {
			return !((this->left > rect.right)
				|| (this->top > rect.bottom)
				|| (this->right < rect.left)
				|| (this->bottom < rect.top));
		}

		void SetX(const T x) {
			*this = sRect1<T>(x, top, x + Width(), bottom);
		}
		void SetY(const T y) {
			*this = sRect1(left, y, right, y + Height());
		}	
		void SetWidth(const T width) {
			*this = Rect(left, top, width, Height());
		}
		void SetHeight(const T height) {
			*this = sRect1(left, top, right, height);
		}
		void Offset(const T x, const T y) {
			this->left += x;
			this->top += y;
			this->right += x;
			this->bottom += y;
		}
		T Width() const {
			return abs(right - left);
		}
		T Height() const {
			return abs(bottom - top);
		}
		Vector2 Center() const {
			return Vector2((float)(left + right)*0.5f, (float)(top + bottom)*0.5f);
		}
		Vector3 Center3D() const {
			return Vector3((float)(left + right)*0.5f, 0, (float)(top + bottom)*0.5f);
		}
		bool IsEmpty() const {
			return (0 == left) && (0 == right) && (0 == top) && (0 == bottom);
		}

		sRect1 operator-(const sRect1 &rhs) {
			return sRect1(left - rhs.left, 
				top - rhs.top, 
				right - rhs.right, 
				bottom - rhs.bottom);
		}
		sRect1 operator+(const sRect1 &rhs) {
			return sRect1(left + rhs.left,
				top + rhs.top,
				right + rhs.right,
				bottom + rhs.bottom);
		}
		sRect1& operator=(const RECT &rhs) {
			left = (T)rhs.left;
			right = (T)rhs.right;
			top = (T)rhs.top;
			bottom = (T)rhs.bottom;
			return *this;
		}
		bool operator==(const sRect1 &rhs) {
			return (left == rhs.left)
				&& (right == rhs.right)
				&& (top == rhs.top)
				&& (bottom == rhs.bottom);
		}

		T left, top, right, bottom;
	};

	typedef sRect1<LONG> sRecti;
	typedef sRect1<float> sRectf;
	typedef sRect1<double> sRectd;



	//---------------------------------------------------------------------
	template <class T>
	struct sSize
	{
		T w, h;
	};

	typedef sSize<LONG> sSizei;
	typedef sSize<float> sSizef;


	//---------------------------------------------------------------------
	//template <class T>
	//struct typeVector2
	//{
	//	T x,y;

	//	typeVector2() :x(0), y(0) {}
	//	typeVector2(const T _x, const T _y) : x(_x), y(_y) {}
	//};
	//typedef typeVector2<int> Vector2i;
}
