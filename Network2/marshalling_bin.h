//
// 2019-01-09, jjuiddong
// Marshalling Binary
//
#pragma once


namespace network2
{
	namespace marshalling
	{
		cPacket& AddDelimeter(cPacket &packet);

		cPacket& operator<<(cPacket &packet, const string &rhs);
		template<class T> cPacket& operator<<(cPacket& packet, const T& rhs);
		template<class T, size_t N> cPacket& operator<<(cPacket &packet, const T(&rhs)[N]);
		template<class T> cPacket& operator<<(cPacket& packet, const vector<T> &v);
		template<class T> cPacket& operator<<(cPacket& packet, const list<T> &v);

		cPacket& operator>>(cPacket& packet, OUT string &rhs);
		cPacket& operator>>(cPacket& packet, OUT _variant_t &rhs);
		template<class T> cPacket& operator>>(cPacket& packet, OUT T& rhs);
		template<class T, size_t N> cPacket& operator>>(cPacket &packet, OUT T(&rhs)[N]);
		template<class T> cPacket& operator>>(cPacket& packet, OUT vector<T> &v);
		template<class T> cPacket& operator>>(cPacket& packet, OUT list<T> &v);

		template<class Seq> cPacket& AppendSequence(cPacket &packet, const Seq &seq);
		template<class Seq> void GetSequence(cPacket& packet, OUT Seq& seq);
	}


	//----------------------------------------------------------------------
	// Implements

	inline cPacket& marshalling::AddDelimeter(cPacket &packet)
	{
		// nothing
		return packet;
	}

	// 모든 타입이 binary 단위로 복사되기 때문에, 
	// 생성자 호출은 되지 않는다.
	template<class T>
	inline cPacket& marshalling::operator<<(cPacket& packet, const T& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const string &rhs)
	{
		packet.AppendPtr(rhs.c_str(), rhs.size() + 1);
		return packet;
	}

	template<class T, size_t N>
	inline cPacket& marshalling::operator<<(cPacket &packet, const T(&rhs)[N])
	{
		packet << N;
		for (int i = 0; i < N; ++i)
			packet << rhs[i];
		return packet;
	}


	template<class T>
	inline cPacket& marshalling::operator<<(cPacket& packet, const vector<T> &v)
	{
		AppendSequence(packet, v);
		return packet;
	}

	template<class T>
	inline cPacket& marshalling::operator<<(cPacket& packet, const list<T> &v)
	{
		AppendSequence(packet, v);
		return packet;
	}


	//--------------------------------------------------------------------------
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT string &rhs)
	{
		packet.GetDataString(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, _variant_t &out)
	{
		switch (out.vt)
		{
		case VT_I2: packet >> out.iVal; break;
		case VT_I4: packet >> out.lVal; break;
		case VT_R4: packet >> out.fltVal; break;
		case VT_R8: packet >> out.dblVal; break;

		case VT_BOOL: packet >> out.bVal; break;
		case VT_DECIMAL: break;
		case VT_I1: packet >> out.cVal; break;
		case VT_UI1: packet >> out.bVal; break;
		case VT_UI2: packet >> out.uiVal; break;
		case VT_UI4: packet >> out.ulVal; break;
		case VT_I8: packet >> out.llVal; break;
		case VT_UI8: packet >> out.ullVal; break;

		case VT_INT: packet >> (out.intVal); break;
		case VT_UINT: packet >> (out.uintVal); break;

		case VT_BSTR:
		{
			string str;
			packet >> (str);
#ifdef _UNICODE
			out.bstrVal = (_bstr_t)common::str2wstr(str).c_str();
#else
			out.bstrVal = (_bstr_t)str.c_str();
#endif
		}
		break;

		default:
			break;
		}
		return packet;
	}
	
	template<class T>
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT T& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}


	template<class T, size_t N>
	inline cPacket& marshalling::operator>>(cPacket &packet, OUT T(&rhs)[N])
	{
		int size = 0;
		packet >> size;
		for (int i = 0; i < size && i < N; ++i)
			packet >> rhs[i];
		return packet;
	}


	template<class T>
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT std::vector<T> &v)
	{
		GetSequence(packet, v);
		return packet;
	}

	template<class T>
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT std::list<T> &v)
	{
		GetSequence(packet, v);
		return packet;
	}


	template<class Seq>
	inline cPacket& marshalling::AppendSequence(cPacket &packet, const Seq &seq)
	{
		packet << (int)seq.size();
		for (auto &v : seq)
			packet << v;
		return packet;
	}

	template<class Seq>
	inline void marshalling::GetSequence(cPacket& packet, OUT Seq& seq)
	{
		int size = 0;
		packet >> size;
		if (size < 0)
			return; // todo: log, error!! 
		if (size > 10000)
			return; // todo: too many size data in packet, log, error!!	

		typedef Seq::value_type type;
		seq.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			type t;
			packet >> t;
			seq.push_back(t);
		}
	}

}
