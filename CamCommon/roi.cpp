
#include "stdafx.h"
#include "roi.h"
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace cv;
using namespace cvproc;

cRoi::cRoi()
{
	m_rect = cvRect(0, 0, 0, 0);
}

cRoi::cRoi(const cv::Rect &rect)
{
}


Mat& cRoi::Update(const Mat &src)
{
	RETV(src.empty(), m_final);

	m_final = src.clone();

	if (m_rect.width == 0)
		return m_final;

	m_final(m_rect).copyTo(m_matRoi);
	m_final.setTo(Scalar(0, 0, 0));

	for (int i = 0; i < m_rect.height; ++i)
	{
		uchar *pdst = m_final.data + m_rect.x*m_final.elemSize() + (m_final.step * (m_rect.y + i));
		uchar *psrc = m_matRoi.data + (m_matRoi.step * i);
		CopyMemory(pdst, psrc, m_matRoi.step);
	}

	return m_final;
}


bool cRoi::Read(const string &fileName)
{
	using namespace std;
	cout << "read roi file... ";

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName, props);

		if (props.get<string>("format", "") != "roi rect")
		{
			cout << "Fail " << endl;
			return false;
		}

		m_rect.x = props.get<int>("x", 0);
		m_rect.y = props.get<int>("y", 0);
		m_rect.width = props.get<int>("width", 0);
		m_rect.height = props.get<int>("height", 0);

		cout << "Ok" << endl;
	}
	catch (std::exception&e)
	{
		cout << "Fail " << e.what() << endl;
		return false;
	}

	return true;
}


bool cRoi::Write(const string &fileName)
{
	using namespace std;
	cout << "write roi rect file... ";

	if (fileName.empty())
		return false;

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;

		props.add<string>("format", "roi rect");

		props.add<int>("x", m_rect.x);
		props.add<int>("y", m_rect.y);
		props.add<int>("width", m_rect.width);
		props.add<int>("height", m_rect.height);

		boost::property_tree::write_json(fileName, props);
	}
	catch (std::exception&e)
	{
		cout << "Fail " << e.what() << endl;
		return false;
	}

	return true;
}
