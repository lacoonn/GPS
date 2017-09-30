// DrivingInGpp.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

using namespace std;

int main()
{
	DrivingInGpp();

    return 0;
}

void DrivingInGpp()
{
	vector<Gps> gppPath; // 여기에 지나가야할 gps 좌표 리스트가 저장되어 있다고 가정하자
	int gppPathSize = gppPath.size();

	for (int currentIndex = 0, nextIndex = 1; nextIndex < gppPathSize; currentIndex++, nextIndex++) // 패스를 다 돌때까지
	{
		Gps currentGps = getGps();
		Imu currentImu = getImu();

		Gps currentDestination = gppPath.at(currentIndex);
		Gps nextDestination = gppPath.at(nextIndex);

		double currentBearing = currentImu.bearing; // 현재 향하고 있는 방위각
		double nextBearing = getBearing(gppPath.at(currentIndex), gppPath.at(nextIndex)); // 다음 목적지를 위해 향해야할 방위각
		double differenceBearing = sqrt(nextBearing - currentBearing); // 다음 방위각과 현재 방위각의 차이(절대값)

		bool shouldRotate = false; // 다음 목적지를 위해서 회전해야하는지 여부
		if (30 < differenceBearing && differenceBearing < 150) // 회전해야하는 경우(90 인근값)
			shouldRotate = true;
		if (210 < differenceBearing && differenceBearing < 330) // 회전해야하는 경우(270 인근값)
			shouldRotate = true;

		while (1) // 목표 위치에 도달하기 전까지 반복
		{
			currentGps = getGps();
			currentImu = getImu();

			double distanceToDestination = getDistance(currentGps, currentDestination); // 목표지와 현재 위치의 거리를 계산


			if (shouldRotate == false && distanceToDestination < ARRIVE_DISTANCE) // 목표 위치에 도착(근접)
			{
				// 정지시킬 필요 없음
				break;
			}
			if (shouldRotate == true && distanceToDestination < ROTATE_DISTANCE) // 회전을 위해 목표 위치보다 조금 덜 근접
			{
				// 회전(현재방위각 --> 목표방위각)
				break;
			}
			else
			{
				// 가던 방향으로 직진
			}
		}
	}

	/*
	마지막 목적지까지 이동하는 부분 필요
	회전까지 완료되어 있기 때문에 직진 이동만 하면 됨
	*/

	while (1) // 목표 위치에 도달하기 전까지 반복
	{
		Gps currentIndex = gppPath.back();

		gps_mutex.lock();
		Gps currentGps = g_gps;
		gps_mutex.unlock();

		double distanceToDestination = getDistance(currentGps, currentIndex); // 목표지와 현재 위치의 거리를 계산

		if (distanceToDestination < ARRIVE_DISTANCE) // 목표 위치에 도착(근접)
		{
			// 반복문 종료
			// GPP 종료(멈추나?)
			break;
		}
	}
}