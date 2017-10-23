#pragma once

#include "DataStructure.h"
#include <cmath>

double deg2rad(double deg)
{
	return (deg * 3.141592 / 180.0);
}

// This function converts radians to decimal degrees
double rad2deg(double rad)
{
	return (rad * 180 / 3.141592);
}


double getDistance(GpsData point1, GpsData point2)
{
	// 두 점(위도, 경도) 사이의 거리를 구한다
	double lat1 = point1.latitude;
	double lon1 = point1.longitude;
	double lat2 = point2.latitude;
	double lon2 = point2.longitude;

	if (lat1 == lat2 && lon1 == lon2)
		return 0;

	double theta = lon1 - lon2;
	double dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));

	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;

	// meter
	//dist = dist * 1609.344;

	//centimeter
	dist = dist * 160934.4;

	return dist;
}


/*double getBearing(GpsData point1, GpsData point2) // 방위각을 구한다! (point1에서 point2를 향하는 각도)
{
	double P1_latitude = point1.latitude;
	double P1_longitude = point1.longitude;
	double P2_latitude = point2.latitude;
	double P2_longitude = point2.longitude;

	if (P1_latitude == P2_latitude && P1_longitude == P2_longitude) // 좌표가 같을 경우(방향을 구할 수 없을 경우) -1을 반환한다.
		return -1;

	// 현재 위치 : 위도나 경도는 지구 중심을 기반으로 하는 각도이기 때문에 라디안 각도로 변환한다.
	double Cur_Lat_radian = P1_latitude * (3.141592 / 180);
	double Cur_Lon_radian = P1_longitude * (3.141592 / 180);


	// 목표 위치 : 위도나 경도는 지구 중심을 기반으로 하는 각도이기 때문에 라디안 각도로 변환한다.
	double Dest_Lat_radian = P2_latitude * (3.141592 / 180);
	double Dest_Lon_radian = P2_longitude * (3.141592 / 180);

	// radian distance
	double radian_distance = 0;
	radian_distance = acos(sin(Cur_Lat_radian) * sin(Dest_Lat_radian) + cos(Cur_Lat_radian) * cos(Dest_Lat_radian) * cos(Cur_Lon_radian - Dest_Lon_radian));

	if (radian_distance == 0) // 거리가 0일 경우(방향을 구할 수 없을 경우) -1을 반환한다.
		return -1;

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
}*/

double getBearing(GpsData from, GpsData to) // 방위각을 구한다! (point1에서 point2를 향하는 각도)
{
	double PI = 3.141592;
	double fromLat = deg2rad(from.latitude);
	double fromLon = deg2rad(from.longitude);
	double toLat = deg2rad(to.latitude);
	double toLon = deg2rad(to.longitude);

	double dLon = toLon - fromLon;

	double dPhi = log(tan(toLat / 2.0 + 3.141592 / 4.0) / tan(fromLat / 2.0 + 3.141592 / 4.0));
	if (abs(dLon > 3.141592))
	{
		if (dLon > 0.0)
			dLon = -(2.0 * PI - dLon);
		else
			dLon = (2.0 * PI + dLon);
	}

	double result = rad2deg(atan2(dLon, dPhi)) + 360.0;
	result = (int)result % 360;

	return result;
}