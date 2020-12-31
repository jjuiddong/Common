//
// 2020-11-10, jjuiddong
// marshalling json
//
#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace network2
{
	// declare marshalling function
	namespace marshalling_json
	{
		cPacket& operator<<(cPacket &packet, const string &rhs);

		using boost::property_tree::ptree;

		ptree& put(ptree &props, const char *typeName, const bool rhs);
		ptree& put(ptree &props, const char *typeName, const char rhs);
		ptree& put(ptree &props, const char *typeName, const unsigned char rhs);
		ptree& put(ptree &props, const char *typeName, const short rhs);
		ptree& put(ptree &props, const char *typeName, const unsigned short rhs);
		ptree& put(ptree &props, const char *typeName, const int rhs);
		ptree& put(ptree &props, const char *typeName, const unsigned int rhs);
		ptree& put(ptree &props, const char *typeName, const long rhs);
		ptree& put(ptree &props, const char *typeName, const unsigned long rhs);
		ptree& put(ptree &props, const char *typeName, const float rhs);
		ptree& put(ptree &props, const char *typeName, const double rhs);
		ptree& put(ptree &props, const char *typeName, const int64 rhs);
		ptree& put(ptree &props, const char *typeName, const uint64 rhs);
		ptree& put(ptree &props, const char *typeName, const string &rhs);
		ptree& put(ptree &props, const char *typeName, const Vector3 &rhs);

		template<class T, size_t N> ptree& put(ptree &props, const char *typeName, const T(&rhs)[N]);
		template<class T> ptree& put(ptree &props, const char *typeName, const vector<T> &v);
		template<class T> ptree& put(ptree &props, const char *typeName, const list<T> &v);


		cPacket& operator>>(cPacket &packet, OUT string &rhs);

		ptree& get(ptree &props, const char *typeName, OUT bool &rhs);
		ptree& get(ptree &props, const char *typeName, OUT char &rhs);
		ptree& get(ptree &props, const char *typeName, OUT unsigned char &rhs);
		ptree& get(ptree &props, const char *typeName, OUT short &rhs);
		ptree& get(ptree &props, const char *typeName, OUT unsigned short &rhs);
		ptree& get(ptree &props, const char *typeName, OUT int &rhs);
		ptree& get(ptree &props, const char *typeName, OUT unsigned int &rhs);
		ptree& get(ptree &props, const char *typeName, OUT long &rhs);
		ptree& get(ptree &props, const char *typeName, OUT unsigned long &rhs);
		ptree& get(ptree &props, const char *typeName, OUT float &rhs);
		ptree& get(ptree &props, const char *typeName, OUT double &rhs);
		ptree& get(ptree &props, const char *typeName, OUT int64 &rhs);
		ptree& get(ptree &props, const char *typeName, OUT uint64 &rhs);
		ptree& get(ptree &props, const char *typeName, OUT string &rhs);
		ptree& get(ptree &props, const char *typeName, OUT Vector3 &rhs);
		ptree& get(ptree &props, const char *typeName, OUT _variant_t &rhs);

		template<class T, size_t N> ptree& get(ptree &props, const char *typeName, OUT T(&rhs)[N]);
		template<class T> ptree& get(ptree &props, const char *typeName, OUT vector<T> &v);
		template<class T> ptree& get(ptree &props, const char *typeName, OUT list<T> &v);
	}


	//----------------------------------------------------------------------
	// Implements marshalling function
	namespace marshalling_json {
		using boost::property_tree::ptree;

		inline cPacket& marshalling_json::operator<<(cPacket &packet, const string &rhs)
		{
			packet.AppendPtr(rhs.c_str(), rhs.size());
			return packet;
		}

		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const bool rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const char rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const unsigned char rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const short rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const unsigned short rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const int rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const unsigned int rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const long rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const unsigned long rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const float rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const double rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const int64 rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const uint64 rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const string &rhs) {
			return props.put(typeName, rhs);
		}
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const Vector3 &rhs) {
			props.put(string(typeName)+".x", rhs.x);
			props.put(string(typeName)+".y", rhs.y);
			props.put(string(typeName)+".z", rhs.z);
			return props;
		}

		template<class T, size_t N>
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const T(&rhs)[N])
		{
			for (int i = 0; i < N; ++i)
				put(props, "", rhs[i]);
			return props;
		}

		template<class T>
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const vector<T> &v)
		{
			return props;
		}

		template<class T>
		inline ptree& marshalling_json::put(ptree &props, const char *typeName, const list<T> &v)
		{
			return props;
		}


		//-----------------------------------------------------------------------
		inline cPacket& marshalling_json::operator>>(cPacket &packet, OUT string &rhs)
		{
			packet.GetDataString(rhs);
			return packet;
		}

		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT bool &out) {
			out = props.get<bool>(typeName, false);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT char &out) {
			out = props.get<char>(typeName, ' ');
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT unsigned char &out) {
			out = props.get<unsigned char>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT short &out) {
			out = props.get<short>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT unsigned short &out) {
			out = props.get<unsigned short>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT int &out) {
			out = props.get<int>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT unsigned int &out) {
			out = props.get<unsigned int>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT long &out) {
			out = props.get<long>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT unsigned long &out) {
			out = props.get<unsigned long>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT float &out) {
			out = props.get<float>(typeName, 0.f);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT double &out) {
			out = props.get<double>(typeName, 0.f);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT int64 &out) {
			out = props.get<int64>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT uint64 &out) {
			out = props.get<uint64>(typeName, 0);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT string &out) {
			out = props.get<string>(typeName, "");
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT Vector3 &out) {
			out.x = props.get<float>(string(typeName) + ".x", 0.f);
			out.y = props.get<float>(string(typeName) + ".y", 0.f);
			out.z = props.get<float>(string(typeName) + ".z", 0.f);
			return props;
		}
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT _variant_t &out) {
			switch (out.vt)
			{
			case VT_I2: get(props, typeName, out.iVal); break;
			case VT_I4: get(props, typeName, out.lVal); break;
			case VT_R4: get(props, typeName, out.fltVal); break;
			case VT_R8: get(props, typeName, out.dblVal); break;
			
			case VT_BOOL: get(props, typeName, out.bVal); break;
			case VT_DECIMAL: break;
			case VT_I1: get(props, typeName, out.cVal); break;
			case VT_UI1: get(props, typeName, out.bVal); break;
			case VT_UI2: get(props, typeName, out.uiVal); break;
			case VT_UI4: get(props, typeName, out.ulVal); break;
			case VT_I8: get(props, typeName, out.llVal); break;
			case VT_UI8: get(props, typeName, out.ullVal); break;
			
			case VT_INT: get(props, typeName, out.intVal); break;
			case VT_UINT: get(props, typeName, out.uintVal); break;
			
			case VT_BSTR:
			{
				string str;
				get(props, typeName, str);
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
			return props;
		}

		template<class T, size_t N>
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT T(&rhs)[N]) {
			for (int i = 0; i < N; ++i)
				get(props, typeName, rhs[i]);
			return props;
		}
		template<class T>
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT vector<T> &v) {
			return props;
		}	
		template<class T> 
		inline ptree& marshalling_json::get(ptree &props, const char *typeName, OUT list<T> &v) {
			return props;
		}

	}

}
