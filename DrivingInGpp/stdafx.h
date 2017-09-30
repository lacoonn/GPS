// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



#include <iostream>
#include <vector>
#include <mutex>


#define ARRIVE_DISTANCE 1
#define ROTATE_DISTANCE 3

struct Gps
{
	int hours;
	int minutes;
	int seconds;
	double latitude;
	double longitude;
	int satelliteInUse;
};

struct Imu
{
	int bearing;
};

double getDistance(Gps point1, Gps point2);
double getBearing(Gps point1, Gps point2);
Gps getGps();
Imu getImu();

std::mutex gps_mutex;
std::mutex imu_mutex;
Gps g_gps; // 멀티스레드를 사용해서 실시간으로 전역 변수를 업데이트한다고 가정
Imu g_imu;

Gps getGps()
{
	gps_mutex.lock();
	Gps tempGps = g_gps;
	gps_mutex.unlock();

	return tempGps;
}

Imu getImu()
{
	imu_mutex.lock();
	Imu tempImu = g_imu;
	imu_mutex.unlock();

	return tempImu;
}

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
