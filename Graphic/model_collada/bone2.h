#pragma once


namespace graphic
{
	
	class cBoneNode2
	{
	public:
		cBoneNode2();
		virtual ~cBoneNode2();

		bool Create(const sRawBone &bone);


	public:
		int m_id;
		string m_name;
		Matrix44 m_localTm;
		Matrix44 m_worldTm;
	};

}
