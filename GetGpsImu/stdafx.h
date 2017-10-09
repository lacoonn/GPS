// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <csignal>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include "ublox.h"
#include "Serial.h"

#define GPXNAME "GPS_TEST"


class GpsData
{
public:
	struct time
	{
		int hours, minutes, seconds;
		uint16_t millis;
		bool valid; //1 = yes, 0 = no
	};
	time datetime;
	double latitude; // 위도
	double longitude; // 경도
	int fixtype; // 0 = invalid, 1 = gps, 2 = dgps
	int satelliteInUse;
	double hdop;
	double altitude;


	GpsData()
	{
		latitude = 0;
		longitude = 0;
		satelliteInUse = 0;
	}

	void setData(time datetime_, double latitude_, double longitude_, int fixtype_, int satelliteInUse_, double hdop_, double altitude_) {
		datetime = datetime_;
		latitude = latitude_; // 위도
		longitude = longitude_; // 경도
		fixtype = fixtype_;
		satelliteInUse = satelliteInUse_;
		hdop = hdop_;
		altitude = altitude_;
	}
};

class GpxFileManager
{
public:
	std::ofstream gpxFileStream;

	GpxFileManager()
	{

	}

	void openFileStream(std::string _filename)
	{
		std::string tempString = _filename;
		tempString.append(".gpx");
		gpxFileStream.open(tempString);

		std::string gpxname = GPXNAME; // gpx 파일 xml <name>

		gpxFileStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
		gpxFileStream << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xalan=\"http://xml.apache.org/xalan\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" creator=\"MotionX Live\" version=\"1.1\">" << std::endl;
		gpxFileStream << "  <trk>" << std::endl;
		gpxFileStream << "    <name>" << gpxname.c_str() << "</name>" << std::endl;
		gpxFileStream << "    <desc>2016. 5. 11.  7:00 am</desc>" << std::endl;
		//(int)datetime.hours, (int)datetime.minutes, (int)datetime.seconds;

		gpxFileStream << "    <trkseg>" << std::endl;
	}


	void writeGpsData(GpsData data)
	{
		gpxFileStream.precision(9);
		gpxFileStream << "      <trkpt lat=\"" << data.latitude << "\" lon=\"" << data.longitude << "\">" << std::endl;
		gpxFileStream << "        <ele></ele>" << std::endl;
		gpxFileStream << "        <time></time>" << std::endl;
		gpxFileStream << "      </trkpt>" << std::endl;
	}

	void endFileStream()
	{
		gpxFileStream << "    </trkseg>" << std::endl;
		gpxFileStream << "  </trk>" << std::endl;
		gpxFileStream << "</gpx>" << std::endl;

		gpxFileStream.close();
	}
};

class TxtFileManager
{
public:
	std::ofstream txtFileStream;

	TxtFileManager()
	{

	}

	void openFileStream(std::string _filename)
	{
		std::string tempString = _filename;
		tempString.append(".txt");
		txtFileStream.open(tempString);
	}

	void writeGpsData(GpsData data)
	{
		txtFileStream << (int)data.datetime.hours << " " << (int)data.datetime.minutes << " " << (int)data.datetime.seconds << " " <<
			data.latitude << " " << data.longitude << " " << data.fixtype << " " << data.satelliteInUse << " " <<
			data.hdop << " " << data.altitude << std::endl;
	}

	void endFileStream()
	{
		txtFileStream.close();
	}
};


std::string *StringSplit(std::string strTarget, std::string strTok)
{
	int     nCutPos;
	int     nIndex = 0;
	std::string *strResult = new std::string[15];

	while ((nCutPos = strTarget.find_first_of(strTok)) != strTarget.npos)
	{
		if (nCutPos > 0)
		{
			strResult[nIndex++] = strTarget.substr(0, nCutPos);
		}
		strTarget = strTarget.substr(nCutPos + 1);
	}

	if (strTarget.length() > 0)
	{
		strResult[nIndex++] = strTarget.substr(0, nCutPos);
	}

	return strResult;
}

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
