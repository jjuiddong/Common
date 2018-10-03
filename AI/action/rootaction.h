// 루트 액션.
#pragma once

#include "action.h"


namespace ai
{
	template<class T>

	class cRootAction : public cAction<T>
	{
	public:
 		cRootAction(T *agent = NULL)
 			: cAction(agent, "Root", "", eActionType::ROOT)
 		{
 		}
	};

}
