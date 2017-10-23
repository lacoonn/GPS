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
	// �� ��(����, �浵) ������ �Ÿ��� ���Ѵ�
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


/*double getBearing(GpsData point1, GpsData point2) // �������� ���Ѵ�! (point1���� point2�� ���ϴ� ����)
{
	double P1_latitude = point1.latitude;
	double P1_longitude = point1.longitude;
	double P2_latitude = point2.latitude;
	double P2_longitude = point2.longitude;

	if (P1_latitude == P2_latitude && P1_longitude == P2_longitude) // ��ǥ�� ���� ���(������ ���� �� ���� ���) -1�� ��ȯ�Ѵ�.
		return -1;

	// ���� ��ġ : ������ �浵�� ���� �߽��� ������� �ϴ� �����̱� ������ ���� ������ ��ȯ�Ѵ�.
	double Cur_Lat_radian = P1_latitude * (3.141592 / 180);
	double Cur_Lon_radian = P1_longitude * (3.141592 / 180);


	// ��ǥ ��ġ : ������ �浵�� ���� �߽��� ������� �ϴ� �����̱� ������ ���� ������ ��ȯ�Ѵ�.
	double Dest_Lat_radian = P2_latitude * (3.141592 / 180);
	double Dest_Lon_radian = P2_longitude * (3.141592 / 180);

	// radian distance
	double radian_distance = 0;
	radian_distance = acos(sin(Cur_Lat_radian) * sin(Dest_Lat_radian) + cos(Cur_Lat_radian) * cos(Dest_Lat_radian) * cos(Cur_Lon_radian - Dest_Lon_radian));

	if (radian_distance == 0) // �Ÿ��� 0�� ���(������ ���� �� ���� ���) -1�� ��ȯ�Ѵ�.
		return -1;

	// ������ �̵� ������ ���Ѵ�.(���� ��ǥ���� ���� ��ǥ�� �̵��ϱ� ���ؼ��� ������ �����ؾ� �Ѵ�. ���Ȱ��̴�.
	double radian_bearing = acos((sin(Dest_Lat_radian) - sin(Cur_Lat_radian) * cos(radian_distance)) / (cos(Cur_Lat_radian) * sin(radian_distance)));        // acos�� �μ��� �־����� x�� 360�й��� ������ �ƴ� radian(ȣ��)���̴�.

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

double getBearing(GpsData from, GpsData to) // �������� ���Ѵ�! (point1���� point2�� ���ϴ� ����)
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