//
// 2019-01-09, jjuiddong
// marshalling Ascii
//
// 2019-01-11
//	- slow ascii conversion, need optimize
//
#pragma once


namespace network2
{
	namespace marshalling_ascii
	{
		static const char DELIMETER = ',';
		static const char ARRAY_DELIMETER = ';';

		cPacket& AddDelimeter(cPacket &packet);

		cPacket& operator<<(cPacket &packet, const bool rhs);
		cPacket& operator<<(cPacket &packet, const char rhs);
		cPacket& operator<<(cPacket &packet, const unsigned char rhs);
		cPacket& operator<<(cPacket &packet, const short rhs);
		cPacket& operator<<(cPacket &packet, const unsigned short rhs);
		cPacket& operator<<(cPacket &packet, const int rhs);
		cPacket& operator<<(cPacket &packet, const unsigned int rhs);
		cPacket& operator<<(cPacket& packet, const long rhs);
		cPacket& operator<<(cPacket& packet, const unsigned long rhs);
		cPacket& operator<<(cPacket &packet, const float rhs);
		cPacket& operator<<(cPacket &packet, const double rhs);
		cPacket& operator<<(cPacket &packet, const int64 rhs);
		cPacket& operator<<(cPacket &packet, const uint64 rhs);
		cPacket& operator<<(cPacket &packet, const string &rhs);
		template<class T, size_t N> cPacket& operator<<(cPacket &packet, const T (&rhs)[N]);
		template<class T> cPacket& operator<<(cPacket &packet, const vector<T> &v);
		template<class T> cPacket& operator<<(cPacket &packet, const list<T> &v);

		cPacket& operator>>(cPacket &packet, OUT bool& rhs);
		cPacket& operator>>(cPacket &packet, OUT char& rhs);
		cPacket& operator>>(cPacket &packet, OUT unsigned char& rhs);
		cPacket& operator>>(cPacket &packet, OUT short& rhs);
		cPacket& operator>>(cPacket &packet, OUT unsigned short& rhs);
		cPacket& operator>>(cPacket &packet, OUT int& rhs);
		cPacket& operator>>(cPacket &packet, OUT unsigned int& rhs);
		cPacket& operator>>(cPacket& packet, OUT long& rhs);
		cPacket& operator>>(cPacket& packet, OUT unsigned long& rhs);
		cPacket& operator>>(cPacket &packet, OUT float& rhs);
		cPacket& operator>>(cPacket &packet, OUT double& rhs);
		cPacket& operator>>(cPacket &packet, OUT int64& rhs);
		cPacket& operator>>(cPacket &packet, OUT uint64& rhs);
		cPacket& operator>>(cPacket &packet, OUT string &rhs);
		cPacket& operator>>(cPacket& packet, OUT _variant_t &rhs);
		template<class T, size_t N> cPacket& operator>>(cPacket &packet, OUT T(&rhs)[N]);
		template<class T> cPacket& operator>>(cPacket &packet, OUT vector<T> &v);
		template<class T> cPacket& operator>>(cPacket &packet, OUT list<T> &v);

		template<class Seq> cPacket& AppendSequence(cPacket &packet, const char delimeter, const Seq &seq);
		template<class Seq> cPacket& GetSequence(cPacket &packet, const char delimeter, OUT Seq& seq);
	}


	//----------------------------------------------------------------------
	// Implements

	inline cPacket& marshalling_ascii::AddDelimeter(cPacket &packet)
	{
		packet.AppendDelimeter(DELIMETER);
		return packet;
	}


#define SPRINTF_FORMAT(fmt, val) \
	char buff[32]; \
	const int len = sprintf_s(buff, fmt, val); \
	packet.AppendPtr(buff, len); \
	return packet;


	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const bool rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const char rhs)
	{
		SPRINTF_FORMAT("%c", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const unsigned char rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const short rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const unsigned short rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const int rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const unsigned int rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const long rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const unsigned long rhs)
	{
		SPRINTF_FORMAT("%d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const float rhs)
	{
		SPRINTF_FORMAT("%f", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const double rhs)
	{
		SPRINTF_FORMAT("%f", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const int64 rhs)
	{
		SPRINTF_FORMAT("%I64d", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const uint64 rhs)
	{
		SPRINTF_FORMAT("%I64u", rhs);
	}

	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const string &rhs)
	{
		packet.AppendPtr(rhs.c_str(), rhs.size());
		return packet;
	}

	template<class T, size_t N>
	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const T(&rhs)[N])
	{
		for (int i = 0; i < N; ++i)
		{
			packet << rhs[i];
			if (i < N - 1)
				packet.AppendDelimeter(ARRAY_DELIMETER);
		}
		return packet;
	}

	template<class T>
	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const vector<T> &v)
	{
		return AppendSequence(packet, ARRAY_DELIMETER, v);
	}

	template<class T>
	inline cPacket& marshalling_ascii::operator<<(cPacket &packet, const list<T> &v)
	{
		return AppendSequence(packet, ARRAY_DELIMETER, v);
	}


	//-----------------------------------------------------------------------
#define GETASCII_FORMAT(expr) \
	char buff[32] = {NULL, }; \
	packet.GetDataAscii(DELIMETER, ARRAY_DELIMETER, buff, ARRAYSIZE(buff)); \
	expr;\
	return packet;


	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT bool& rhs)
	{
		GETASCII_FORMAT( rhs = (atoi(buff) > 0)? true : false );
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT char& rhs)
	{
		GETASCII_FORMAT(rhs = buff[0]);
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT unsigned char& rhs)
	{
		GETASCII_FORMAT(rhs = (unsigned char)buff[0]);
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT short& rhs)
	{
		GETASCII_FORMAT(rhs = (short)atoi(buff));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT unsigned short& rhs)
	{
		GETASCII_FORMAT(rhs = (unsigned short)strtoul(buff, NULL, 0));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT int& rhs)
	{
		GETASCII_FORMAT(rhs = atoi(buff));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT unsigned int& rhs)
	{
		GETASCII_FORMAT(rhs = (unsigned int)strtoul(buff, NULL, 0));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT long& rhs)
	{
		GETASCII_FORMAT(rhs = atoi(buff));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT unsigned long& rhs)
	{
		GETASCII_FORMAT(rhs = (unsigned int)strtoul(buff, NULL, 0));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT float& rhs)
	{
		GETASCII_FORMAT(rhs = (float)atof(buff));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT double& rhs)
	{
		GETASCII_FORMAT(rhs = (double)atof(buff));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT int64& rhs)
	{
		GETASCII_FORMAT(rhs = (int64)_atoi64(buff));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT uint64& rhs)
	{
		GETASCII_FORMAT(rhs = (uint64)strtoull(buff, NULL, 0));
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT string &rhs)
	{
		packet.GetDataString(DELIMETER, ARRAY_DELIMETER,rhs);
		return packet;
	}

	inline cPacket& marshalling_ascii::operator>>(cPacket& packet, OUT _variant_t &out)
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

	template<class T, size_t N>
	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT T(&rhs)[N])
	{
		for (int i = 0; i < N; ++i)
		{
			packet >> rhs[i];

			if (packet.m_lastDelim != ARRAY_DELIMETER)
				break;
		}
		return packet;
	}

	template<class T>
	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT std::vector<T> &v)
	{
		return GetSequence(packet, ARRAY_DELIMETER, v);
	}

	template<class T>
	inline cPacket& marshalling_ascii::operator>>(cPacket &packet, OUT std::list<T> &v)
	{
		return GetSequence(packet, ARRAY_DELIMETER, v);
	}


	template<class Seq>
	inline cPacket& marshalling_ascii::AppendSequence(cPacket &packet, const char delimeter, const Seq &seq)
	{
		MARSHALLING_ASCII_APPEND_SEQ(packet, delimeter, seq);
	}

	template<class Seq>
	inline cPacket& marshalling_ascii::GetSequence(cPacket &packet, const char delimeter, OUT Seq& seq)
	{
		MARSHALLING_ASCII_GET_SEQ(packet, delimeter, Seq, seq);
	}

}
