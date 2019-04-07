//
// 2019-01-09, jjuiddong
// protocol macro
//
#pragma once


namespace network2
{
	typedef vector<iProtocolHandler*> ProtocolHandlers;

	// cProtocolDispatcher::Dispatch 에서 사용되는 매크로
#define SEND_HANDLER(HandlerType, handlers, func) \
	for (auto &p : handlers) \
	{ \
		HandlerType *handler = dynamic_cast<HandlerType*>(p); \
		if (handler) \
			handler->func; \
	}


	// 템플릿 인자 T 타입과 같은 프로토콜을 matchHandlers 에 넣어 리턴한다.
	// @return if matchHandlers is empty return false, or return true 
	template<class T>
	inline bool HandlerMatching(const ProtocolHandlers &handlers, OUT ProtocolHandlers &matchHandlers)
	{
		for (auto &p : handlers)
		{
			T *handler = dynamic_cast<T*>(p);
			if (handler)
				matchHandlers.push_back(handler);
		}
		return !matchHandlers.empty();
	}

}
