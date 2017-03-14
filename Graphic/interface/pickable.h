//
// 2017-03-14, jjuiddong
// Picking Object Interface
//
#pragma once


namespace graphic
{

	interface iPickable
	{
	public:
		iPickable() {}
		virtual ~iPickable() {}
		virtual bool Pick(const Vector3 &orig, const Vector3 &dir) = 0;
		virtual void OnPicking() = 0; // Picking Success Event
	};

}
