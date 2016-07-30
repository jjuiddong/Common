//
// 2016-01-20, jjuiddong
//
// Frame per Seconds 측정 클래스
//
//
#pragma once

#include <mmsystem.h>


namespace cvproc
{
	
	class cFps
	{
	public:
		cFps() : m_lastUpdateTime(0), m_fps(0), m_inc(0)
		{
		}

		virtual ~cFps() {}

		int Update()
		{
			CalcFps();
			return m_fps;
		}


		void Update(cv::Mat &dst)
		{
			CalcFps();
			cv::putText(dst, common::format("%d", m_fps), cv::Point(10, 30), 1, 2, cv::Scalar(255, 255, 255));
		}


		void UpdateConsole()
		{
			if (CalcFps())
			{
				std::cout << "FPS : " << m_fps << std::endl;
			}
		}

		bool UpdateSeconds(OUT int &out)
		{
			const bool reval = CalcFps();
			out = m_fps;
			return reval;
		}


	protected:
		bool CalcFps()
		{
			const int curT = timeGetTime();
			if (0 == m_lastUpdateTime)
				m_lastUpdateTime = curT;

			++m_inc;
			if (curT - m_lastUpdateTime > 1000)
			{
				m_fps = m_inc;
				m_inc = 0;
				m_lastUpdateTime = curT;
				return true;
			}

			return false;
		}

	public:
		int m_lastUpdateTime;
		int m_fps;
		int m_inc;
	};

}
