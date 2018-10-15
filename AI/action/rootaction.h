//
// Root Action
//
//
#pragma once

#include "action.h"


namespace ai
{

	template<class T>
	class cRootAction : public cAction<T>
						, public common::cMemoryPool<cRootAction<T>>
	{
	public:
 		cRootAction(T *agent = NULL)
 			: cAction<T>(agent, "Root", "", eActionType::ROOT)
 		{
 		}
	};

}
