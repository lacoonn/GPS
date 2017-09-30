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

double deg2rad(double deg);
double rad2deg(double rad);
double getDistance(Gps point1, Gps point2);
double getBearing(Gps point1, Gps point2);
Gps getGps();
Imu getImu();
void DrivingInGpp();

std::mutex gps_mutex;
std::mutex imu_mutex;
Gps g_gps; // 멀티스레드를 사용해서 실시간으로 전역 변수를 업데이트한다고 가정
Imu g_imu;

double deg2rad(double deg)
{
	return (deg * 3.141592 / 180.0);
}

// This function converts radians to decimal degrees
double rad2deg(double rad)
{
	return (rad * 180 / 3.141592);
}

double getDistance(Gps point1, Gps point2)
{
	// 두 점(위도, 경도) 사이의 거리를 구한다
	double lat1 = point1.latitude;
	double lon1 = point1.longitude;
	double lat2 = point2.latitude;
	double lon2 = point2.longitude;

	double theta = lon1 - lon2;
	double dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));


	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;

	// meter
	dist = dist * 1609.344;

	//centimeter
	//dist = dist * 160934.4;

	return dist;
}


double getBearing(Gps point1, Gps point2) // 방위각을 구한다! (point1에서 point2를 향하는 각도)
{
	double P1_latitude = point1.latitude;
	double P1_longitude = point1.longitude;
	double P2_latitude = point2.latitude;
	double P2_longitude = point2.longitude;

	// 현재 위치 : 위도나 경도는 지구 중심을 기반으로 하는 각도이기 때문에 라디안 각도로 변환한다.
	double Cur_Lat_radian = P1_latitude * (3.141592 / 180);
	double Cur_Lon_radian = P1_longitude * (3.141592 / 180);


	// 목표 위치 : 위도나 경도는 지구 중심을 기반으로 하는 각도이기 때문에 라디안 각도로 변환한다.
	double Dest_Lat_radian = P2_latitude * (3.141592 / 180);
	double Dest_Lon_radian = P2_longitude * (3.141592 / 180);

	// radian distance
	double radian_distance = 0;
	radian_distance = acos(sin(Cur_Lat_radian) * sin(Dest_Lat_radian) + cos(Cur_Lat_radian) * cos(Dest_Lat_radian) * cos(Cur_Lon_radian - Dest_Lon_radian));

	// 목적지 이동 방향을 구한다.(현재 좌표에서 다음 좌표로 이동하기 위해서는 방향을 설정해야 한다. 라디안값이다.
	double radian_bearing = acos((sin(Dest_Lat_radian) - sin(Cur_Lat_radian) * cos(radian_distance)) / (cos(Cur_Lat_radian) * sin(radian_distance)));        // acos의 인수로 주어지는 x는 360분법의 각도가 아닌 radian(호도)값이다.

	double true_bearing = 0;
	if (sin(Dest_Lon_radian - Cur_Lon_radian) < 0)
	{
		true_bearing = radian_bearing * (180 / 3.141592);
		true_bearing = 360 - true_bearing;
	}
	else
	{
		true_bearing = radian_bearing * (180 / 3.141592);
	}

	return true_bearing;
}

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
