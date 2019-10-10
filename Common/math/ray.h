#pragma once


namespace common
{

	struct Ray
	{
		Ray();
		Ray(const Vector3 &_orig, const Vector3 &_dir);
		Ray(const int x, const int y, const int screenWidth, const int screenHeight,  
			const Matrix44 &matProj, const Matrix44 &matView);

		void Create( const int x, const int y, const int screenWidth, const int screenHeight,  
			const Matrix44 &matProj, const Matrix44 &matView );

		float Distance(const Vector3 &pos);

		Vector3 orig;
		Vector3 dir;
	};

}
