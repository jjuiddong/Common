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

		cPacket& operator<<(cPacket& packet, const bool& rhs);
		cPacket& operator<<(cPacket& packet, const char& rhs);
		cPacket& operator<<(cPacket& packet, const unsigned char& rhs);
		cPacket& operator<<(cPacket& packet, const short& rhs);
		cPacket& operator<<(cPacket& packet, const unsigned short& rhs);
		cPacket& operator<<(cPacket& packet, const int& rhs);
		cPacket& operator<<(cPacket& packet, const unsigned int& rhs);
		cPacket& operator<<(cPacket& packet, const long& rhs);
		cPacket& operator<<(cPacket& packet, const unsigned long& rhs);
		cPacket& operator<<(cPacket& packet, const int64& rhs);
		cPacket& operator<<(cPacket& packet, const uint64& rhs);
		cPacket& operator<<(cPacket& packet, const float& rhs);
		cPacket& operator<<(cPacket& packet, const double& rhs);
		cPacket& operator<<(cPacket &packet, const string &rhs);
		cPacket& operator<<(cPacket& packet, const Vector3 &rhs);
		cPacket& operator<<(cPacket& packet, const _variant_t &rhs);
		template<class T, size_t N> cPacket& operator<<(cPacket &packet, const T(&rhs)[N]);
		template<class T, size_t N> cPacket& operator<<(cPacket &packet, const String<T,N> &rhs);
		template<class T> cPacket& operator<<(cPacket& packet, const vector<T> &v);
		template<class T> cPacket& operator<<(cPacket& packet, const list<T> &v);
		template<class T> cPacket& operator<<(cPacket& packet, const set<T> &v);

		cPacket& operator>>(cPacket& packet, OUT bool& rhs);
		cPacket& operator>>(cPacket& packet, OUT char& rhs);
		cPacket& operator>>(cPacket& packet, OUT unsigned char& rhs);
		cPacket& operator>>(cPacket& packet, OUT short& rhs);
		cPacket& operator>>(cPacket& packet, OUT unsigned short& rhs);
		cPacket& operator>>(cPacket& packet, OUT int& rhs);
		cPacket& operator>>(cPacket& packet, OUT unsigned int& rhs);
		cPacket& operator>>(cPacket& packet, OUT long& rhs);
		cPacket& operator>>(cPacket& packet, OUT unsigned long& rhs);
		cPacket& operator>>(cPacket& packet, OUT int64& rhs);
		cPacket& operator>>(cPacket& packet, OUT uint64& rhs);
		cPacket& operator>>(cPacket& packet, OUT float& rhs);
		cPacket& operator>>(cPacket& packet, OUT double& rhs);
		cPacket& operator>>(cPacket& packet, OUT string &rhs);
		cPacket& operator>>(cPacket& packet, OUT Vector3 &rhs);
		cPacket& operator>>(cPacket& packet, OUT _variant_t &rhs);
		template<class T, size_t N> cPacket& operator>>(cPacket &packet, OUT T(&rhs)[N]);
		template<class T, size_t N> cPacket& operator>>(cPacket &packet, OUT String<T, N> &rhs);
		template<class T> cPacket& operator>>(cPacket& packet, OUT vector<T> &v);
		template<class T> cPacket& operator>>(cPacket& packet, OUT list<T> &v);
		template<class T> cPacket& operator>>(cPacket& packet, OUT set<T> &v);

		template<class Seq> cPacket& AppendSequence(cPacket &packet, const Seq &seq);
		template<class Seq> cPacket& GetSequence(cPacket& packet, OUT Seq& seq);
		template<class Seq> cPacket& GetUnOrderedSequence(cPacket& packet, OUT Seq& seq);
	}


	//----------------------------------------------------------------------
	// Implements

	inline cPacket& marshalling::AddDelimeter(cPacket &packet)
	{
		// nothing
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const bool& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const char& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const unsigned char& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const short& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const unsigned short& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const int& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const long& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const unsigned long& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const unsigned int& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const int64& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const uint64& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const float& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const double& rhs)
	{
		packet.Append(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const string &rhs)
	{
		packet.AppendPtr(rhs.c_str(), rhs.size() + 1);
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const Vector3 &rhs)
	{
		packet << rhs.x;
		packet << rhs.y;
		packet << rhs.z;
		return packet;
	}

	inline cPacket& marshalling::operator<<(cPacket& packet, const _variant_t &rhs)
	{
		packet << rhs.vt; // value type (2 bytes)

		// 4bytes alignments
		switch (rhs.vt)
		{
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
		case VT_INT:
		case VT_UINT:
			packet.Write4ByteAlign();
			break;
		}

		switch (rhs.vt)
		{
		case VT_I2: packet << (short)rhs; break;
		case VT_I4: packet << (int)rhs; break;
		case VT_R4: packet << (float)rhs; break;
		case VT_R8: packet << (double)rhs; break;

		case VT_BOOL: packet << (bool)rhs; break;
		case VT_DECIMAL: break;
		case VT_I1: packet << (char)rhs; break;
		case VT_UI1: packet << (uchar)rhs; break;
		case VT_UI2: packet << (ushort)rhs; break;
		case VT_UI4: packet << (uint)rhs; break;
		case VT_I8: packet << (int64)rhs; break;
		case VT_UI8: packet << (uint64)rhs; break;

		case VT_INT: packet << (int)rhs; break;
		case VT_UINT: packet << (uint)rhs; break;

		case VT_BSTR:
#ifdef _UNICODE
			packet << common::wstr2str(rhs.bstrVal);
#else
			packet << rhs.bstrVal;
#endif
		break;

		default:
			break;
		}
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

	template<class T, size_t N> 
	inline cPacket& marshalling::operator<<(cPacket &packet, const String<T, N> &rhs)
	{
		packet.AppendPtr(rhs.c_str(), rhs.size() + 1);
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

	template<class T>
	inline cPacket& marshalling::operator<<(cPacket& packet, const set<T> &v)
	{
		AppendSequence(packet, v);
		return packet;
	}

	//--------------------------------------------------------------------------
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT bool& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT char& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT unsigned char& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT short& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT unsigned short& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}
	
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT int& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT long& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT unsigned long& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT unsigned int& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT int64& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT uint64& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT float& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT double& rhs)
	{
		packet.GetData(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT string &rhs)
	{
		packet.GetDataString(rhs);
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, OUT Vector3 &rhs)
	{
		packet >> rhs.x;
		packet >> rhs.y;
		packet >> rhs.z;
		return packet;
	}

	inline cPacket& marshalling::operator>>(cPacket& packet, _variant_t &out)
	{
		packet >> out.vt; // value type (2 bytes)

		// 4bytes alignments
		switch (out.vt)
		{
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_UI4:
		case VT_I8:
		case VT_UI8:
		case VT_INT:
		case VT_UINT:
			packet.Read4ByteAlign();
			break;
		}

		switch (out.vt)
		{
		case VT_I2: packet >> out.iVal; break;
		case VT_I4: packet >> out.lVal; break;
		case VT_R4: packet >> out.fltVal; break;
		case VT_R8: packet >> out.dblVal; break;

		case VT_BOOL: 
		{
			bool val;
			packet >> val; 
			out = val;
		}
		break;
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

	template<class T, size_t N>
	inline cPacket& marshalling::operator>>(cPacket &packet, OUT T(&rhs)[N])
	{
		int size = 0;
		packet >> size;
		for (int i = 0; i < size && i < N; ++i)
			packet >> rhs[i];
		return packet;
	}

	template<class T, size_t N> 
	cPacket& marshalling::operator>>(cPacket &packet, OUT String<T, N> &rhs)
	{
		packet.GetDataString(rhs.m_str, N);
		return packet;
	}

	template<class T>
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT std::vector<T> &v)
	{
		return GetSequence(packet, v);
	}

	template<class T>
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT std::list<T> &v)
	{
		return GetSequence(packet, v);
	}

	template<class T>
	inline cPacket& marshalling::operator>>(cPacket& packet, OUT std::set<T> &v)
	{
		return GetUnOrderedSequence(packet, v);
	}


	template<class Seq>
	inline cPacket& marshalling::AppendSequence(cPacket &packet, const Seq &seq)
	{
		MARSHALLING_BIN_APPEND_SEQ(packet, seq);
		return packet;
	}


	template<class Seq>
	inline cPacket& marshalling::GetSequence(cPacket& packet, OUT Seq& seq)
	{
		MARSHALLING_BIN_GET_SEQ(packet, Seq, seq);
		return packet;
	}

	template<class Seq>
	inline cPacket& marshalling::GetUnOrderedSequence(cPacket& packet, OUT Seq& seq)
	{
		MARSHALLING_BIN_GET_SEQ2(packet, Seq, seq);
		return packet;
	}

}
