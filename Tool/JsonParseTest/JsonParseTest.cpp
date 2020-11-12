
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"
#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

struct sParseData {
	string header;
	int length;
	string value;
	vector<int> ar;
};

int parseJsonStr(const string &str);
sParseData parseJsonStrToStruct(const string &str);
string Object2JsonStr(const sParseData &data);

int main()
{
	Poco::Net::HTTPClientSession cs("jjuiddong.iptime.org", 4001);
	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET
		, "/?encoding=text", Poco::Net::HTTPMessage::HTTP_1_1);
	request.set("origin", "http://www.websocket.org");
	Poco::Net::HTTPResponse response;

	string recvStr;
	try {

		Poco::Net::WebSocket* m_psock = new Poco::Net::WebSocket(cs, request, response);
		char const *testStr = "Hello echo websocket!";
		char receiveBuff[256];

		int len = m_psock->sendFrame(testStr, strlen(testStr), Poco::Net::WebSocket::FRAME_TEXT);
		std::cout << "Sent bytes " << len << std::endl;
		int flags = 0;

		int rlen = m_psock->receiveFrame(receiveBuff, 256, flags);
		std::cout << "Received bytes " << rlen << std::endl;
		receiveBuff[rlen] = NULL;
		std::cout << receiveBuff << std::endl;
		recvStr = receiveBuff;

		sParseData data;
		data.header = "ACK";
		data.length = 200;
		data.value = "Convert Object to Json String Data";
		const string str = Object2JsonStr(data);
		m_psock->sendFrame(str.c_str(), str.length(), Poco::Net::WebSocket::FRAME_TEXT);

		m_psock->close();
	}
	catch (std::exception &e) {
		std::cout << "Exception " << e.what();
	}

	// parse receive json string data
	if (recvStr.empty())
		return 0; // finish

	//parseJsonStr(recvStr);
	parseJsonStrToStruct(recvStr);
}


int parseJsonStr(const string &str)
{
	using boost::property_tree::ptree;
	try 
	{
		ptree props;
		stringstream ss(str);
		boost::property_tree::read_json(ss, props);
		ptree &children = props.get_child("");
		for(ptree::value_type &vt : children)
		{
			//const string name = vt.second.get<string>("symbolname");
			//...
			cout << vt.first << " : " << vt.second.data() << endl;
		}
	}
	catch (std::exception &e) 
	{
		cout << e.what() << endl;
	}

	return 0;
}


sParseData parseJsonStrToStruct(const string &str)
{
	using boost::property_tree::ptree;

	sParseData data;
	try
	{
		stringstream ss(str);

		ptree props;
		boost::property_tree::read_json(ss, props);
		ptree &children = props.get_child("");
		
		data.header = children.get<string>("header", "");
		data.length = children.get<int>("length", 0);
		data.value = children.get<string>("value", "");
		cout << data.header << ", " << data.length << ", " << data.value << endl;
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
	}

	return data;
}


string Object2JsonStr(const sParseData &data)
{
	using boost::property_tree::ptree;
	try {
		ptree props;
		props.put("header", data.header);
		props.put("length", data.length);
		props.put("value", data.value);
		//const char *ar[] = { "aa", "bb", "cc" };
		//props.put("ar", ar);

		stringstream ss;
		boost::property_tree::write_json(ss, props);
		return ss.str();
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
	}
	return "";
}
