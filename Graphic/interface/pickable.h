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
		iPickable() : m_isPickEnable(true) {}
		virtual ~iPickable() {}
		bool IsPickEnable() const { return m_isPickEnable; }
		virtual void SetPickEnable(const bool enable) { m_isPickEnable = enable; }
		virtual bool Pick(const Vector3 &orig, const Vector3 &dir) = 0;
		virtual bool Pick2(const Vector3 &orig, const Vector3 &dir, float *pDistance) { return false; }
		virtual void OnPicking() = 0; // Picking Success Event


	public:
		bool m_isPickEnable;
	};

}
