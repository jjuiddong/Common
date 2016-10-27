
#include "stdafx.h"
#include "motionwave.h"


//-------------------------------------------------------------------------------------------
// sMotionData

void sMotionData::clear()
{
	yaw = 0;
	pitch = 0;
	roll = 0;
	heave = 0;
}

sMotionData& sMotionData::operator += (const sMotionData &rhs)
{
	if (this != &rhs)
	{
		yaw += rhs.yaw;
		pitch += rhs.pitch;
		roll += rhs.roll;
		heave += rhs.heave;
	}
	return *this;
}

sMotionData& sMotionData::operator /= (const int n)
{
	yaw /= n;
	pitch /= n;
	roll /= n;
	heave /= n;
	return *this;
}

sMotionData sMotionData::operator*(const float s)
{
	sMotionData data;
	data.yaw = yaw * s;
	data.pitch = pitch * s;
	data.roll = roll * s;
	data.heave = heave * s;
	return data;
}

sMotionData sMotionData::operator+(const sMotionData &rhs)
{
	sMotionData data;
	data.yaw = yaw + rhs.yaw;
	data.pitch = pitch + rhs.pitch;
	data.roll = roll + rhs.roll;
	data.heave = heave + rhs.heave;
	return data;
}

//-------------------------------------------------------------------------------------------




cMotionWave::cMotionWave() :
	m_samplingRate(20)
	, m_isRecord(false)
	, m_isPlay(false)
	, m_recordTime(0)
	, m_recordIncTime(0)
{
	m_samplingTime = 1.f / (float)m_samplingRate;

	// 약 1분가량의 정보를 저장할 수 있는 버퍼를 잡아놓는다.
	Init(m_samplingRate * 60);
}

cMotionWave::~cMotionWave()
{

}


// 모션정보 버퍼를 미리 잡아 놓는다.
void cMotionWave::Init(const int reserveSamplingCount)
{
	if (reserveSamplingCount > 0)
		m_wave.reserve(reserveSamplingCount);
}


// 모션웨이브 파일을 읽어온다.
bool cMotionWave::Read(const string &fileName)
{
	using namespace std;

	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return false;

	char header[4];
	ifs.read(header, 4);

	if ((header[0] != 'M') ||
		(header[1] != 'W') ||
		(header[2] != 'A') ||
		(header[3] != 'V'))
		return false;

	int samplingRate = 0;
	ifs.read((char*)&samplingRate, sizeof(int));

	int samplingCount = 0;
	ifs.read((char*)&samplingCount, sizeof(int));

	m_wave.clear();

	if (samplingCount > 0)
	{
		m_wave.reserve(samplingCount);
		for (int i = 0; i < samplingCount; ++i)
		{
			sMotionData data;
			ifs.read((char*)&data, sizeof(sMotionData));
			m_wave.push_back(data);
		}
	}

	m_samplingRate = samplingRate;
	m_samplingTime = 1.f / (float)m_samplingRate;

	return true;
}


// 모션웨이브 파일을 저장한다.
bool cMotionWave::Write(const string &fileName)
{
	using namespace std;

	ofstream ofs(fileName, ios::binary);
	if (!ofs.is_open())
		return false;

	ofs.write("MWAV", 4);

	const int samplingRate = m_samplingRate;
	ofs.write((char*)&samplingRate, sizeof(int));

	const int samplingCount = m_wave.size();
	ofs.write((char*)&samplingCount, sizeof(int));

	for (int i = 0; i < samplingCount; ++i)
	{
		ofs.write((char*)&m_wave[i], sizeof(sMotionData));
	}

	return true;
}


// 모션파형 정보를 저장하기 시작한다. (초기화 한다)
void cMotionWave::StartRecord()
{
	m_isRecord = true;
	m_recordTime = 0;
	m_storeCount = 0;
	m_wave.clear();

	// 약 1분가량의 정보를 저장할 수 있는 버퍼를 잡아놓는다.
	Init(m_samplingRate * 60);
}


// 모션파형 정보 저장을 종료한다.
void cMotionWave::Stop()
{
	m_isRecord = false;
}


// 모션 정보를 하나씩 저장한다.
// 샘플링시간을 넘겨서, 정보가 저장이 되면 true가 리턴되고, 저장된 값은 out에 넣어서 리턴된다.
// out 이 NULL 이면, 아무일 없다.
bool cMotionWave::Record(const float deltaSeconds, const sMotionData &data, sMotionData *out) // out = NULL
{
	bool isWrite = false; // 값이 저장되면 true가 된다.

	if (m_recordTime == 0)
	{
		m_recordTime = 0.00001f;
		m_recordIncTime = 0.00001f;
		m_wave.push_back(data);

		if (out)
			*out = data;

		m_storeCount = 0;
		m_lastData.clear();
		isWrite = true;
	}
	else
	{
		m_recordTime += deltaSeconds;
		m_recordIncTime += deltaSeconds;
		++m_storeCount;
		m_lastData += data;

		// 샘플링 시간이 넘으면 저장한다.
		if (m_recordIncTime >= m_samplingTime)
		{
			m_lastData /= m_storeCount;
			m_wave.push_back(m_lastData);

			if (out)
				*out = m_lastData;

			//m_recordIncTime -= m_samplingTime;
			m_recordIncTime = 0;
			m_storeCount = 0;
			m_lastData.clear();
			isWrite = true;
		}
	}

	return isWrite;
}


void cMotionWave::StartPlay()
{
	m_isPlay = true;
	m_playTime = 0;
	m_playIncTime = 0;
	m_playIndex = 0;
}


bool cMotionWave::Play(const float deltaSeconds, sMotionData &out)
{
	RETV(!m_isPlay, false);
	RETV(m_wave.empty(), false);

	bool isNextData = false;

	if (m_playTime == 0)
	{
		m_playTime = 0.0001f;
		m_playIncTime = 0.001f;
		out = m_wave[0];
		++m_playIndex;
		isNextData = true;
	}
	else
	{
		m_playTime += deltaSeconds;
		m_playIncTime += deltaSeconds;

		float s = m_playIncTime / m_samplingTime;
		s = min(s, 1); // saturation

		if (m_playIndex < (int)m_wave.size())
		{
			out = (m_wave[m_playIndex] * s) + (m_wave[m_playIndex - 1] * (1 - s));
			isNextData = true;
		}

		// 샘플링 시간이 넘으면 다음 정보를 리턴한다.
		if (m_playIncTime >= m_samplingTime)
		{
			if (m_playIndex < (int)m_wave.size())
			{
				++m_playIndex;
				isNextData = true;
			}

			m_playIncTime -= m_samplingTime;
			if (m_playIncTime > m_samplingTime)
				m_playIncTime = 0;
		}
	}

	return isNextData;
}


void cMotionWave::StopPlay()
{
	m_isPlay = false;
}


// 모션 웨이브를 생성한다.
void cMotionWave::Make(const int samplingRate, const int samplingCount, const sMotionData &data)
{
	m_samplingRate = samplingRate;
	m_samplingTime = 1.f / (float)samplingRate;
	m_storeCount = samplingCount;

	m_wave.clear();
	m_wave.reserve(samplingCount);
	for (int i = 0; i < samplingCount; ++i)
	{
		m_wave.push_back(data);
	}
}


// assign 연산자
cMotionWave& cMotionWave::operator = (const cMotionWave &rhs)
{
	if (this != &rhs)
	{
		m_samplingRate = rhs.m_samplingRate;
		m_samplingTime = rhs.m_samplingTime;

		m_isRecord = false;
		m_wave = rhs.m_wave;

		m_recordTime = 0; // 저장을 시작한 시간이후 부터 경과된 시간을 나타낸다. (Second 단위)
		m_recordIncTime = 0; // m_samplingTime 보다 클 때, 정보를 저장한다.
		m_lastData.clear();
		m_storeCount = 0;

		// Play
		m_playTime = 0;
		m_playIncTime = 0;
		m_isPlay = false;
		m_playIndex = 0;
	}
	return *this;
}


// 모션웨이브를 insertIndex 위치에 size만큼 삽입한다.
bool cMotionWave::Insert(const cMotionWave &src, const int insertIndex, const int size) // size=-1
{
	const int copySize = (size == -1) ? src.m_wave.size() : size;
	if ((int)src.m_wave.size() < copySize)
		return false;

	const int itemCnt = m_wave.size();
	for (int i = 0; i < copySize; ++i)
		m_wave.push_back(sMotionData()); // 임시정보를 추가한다.

	// size 만큼, 오른쪽으로 시프트한다.
	//std::rotate(m_wave.begin() + insertIndex + copySize, m_wave.begin() + insertIndex, m_wave.end());
	if (copySize > 1)
		memmove(&m_wave[insertIndex + copySize - 1], &m_wave[insertIndex], sizeof(m_wave[0]) * itemCnt);

	// insertIndex 위치부터 하나씩 복사한다.
	for (int i = 0; i < copySize; ++i)
	{
		// 예외 처리
		if ((int)m_wave.size() <= i + insertIndex)
			break;
		if ((int)src.m_wave.size() <= i)
			break;

		m_wave[i + insertIndex] = src.m_wave[i];
	}

	return true;
}


// removeIndex위치의 모션웨이브 정보를 size만큼 제거한다.
bool cMotionWave::Remove(const int removeIndex, const int size)
{
	// size 만큼, 왼쪽으로 시프트한다.
	std::rotate(m_wave.begin() + removeIndex, m_wave.begin() + removeIndex + size, m_wave.end());

	// 시프트한 만큼, 제거한다.
	for (int i = 0; i < size; ++i)
		m_wave.pop_back();

	return true;
}
