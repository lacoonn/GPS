// ParseGps.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

using namespace std;

static GpxFileManager gpxFileManager;
static TxtFileManager txtFileManager;
static HANDLE hComm;

int main()
{
	string filename = FILENAME;
	gpxFileManager.openFileStream(filename);
	txtFileManager.openFileStream(filename);

	openSerialPort();

	signal(SIGINT, handler);

	DWORD NoBytesRead;
	bool check_tmp = false;

	while (true) {
		char line[256];
		char data;
		Ublox gps;

		line[0] = 1;

		//file_in.getline(line, sizeof(line));

		while (true) {
			ReadFile(hComm,           //Handle of the Serial port
				&data,       //Temporary character
				sizeof(data),//Size of TempChar
				&NoBytesRead,    //Number of bytes read
				NULL);

			if (NoBytesRead > 0)
				gps.encode(data); // 한 라인을 입력받으면 gps 변수 값이 없데이트된다.

			if (data == '\n') // 한 라인을 입력받으면 라인버퍼를 초기화한다.
			{
				if (gps.lastMessage == gps.GGA) {
					GpsData::time datetime;
					datetime.hours = (int)gps.datetime.hours;
					datetime.minutes = (int)gps.datetime.minutes;
					datetime.seconds = (int)gps.datetime.seconds;
					double latitude = (double)gps.latitude;
					double longitude = (double)gps.longitude;
					int fixtype = (int)gps.fixtype;
					int satelliteInUse = (int)gps.sats_in_use;
					double hdop = (double)gps.hdop;
					double altitude = (double)gps.altitude;

					printf("%d:%d:%d, %lf, %lf, %d, %d, %lf, %lf\n", datetime.hours, datetime.minutes, datetime.seconds, latitude, longitude, fixtype, satelliteInUse, hdop, altitude);

					GpsData tempData;
					tempData.setData(datetime, latitude, longitude, fixtype, satelliteInUse, hdop, altitude);
					gpxFileManager.writeGpsData(tempData);
					txtFileManager.writeGpsData(tempData);
				}
				break;
			}
		}
	}

	// Close HANDLE Comm
	CloseHandle(hComm);

	return 0;
}

bool openSerialPort()
{
	wchar_t str[1000] = PORTNUM;

	hComm = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("Error in opening serial port\n");

		return false;
	}
	else {
		printf("opening serial port successful\n");


		DCB dcbSerialParams = { 0 }; // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		bool Status = GetCommState(hComm, &dcbSerialParams);

		dcbSerialParams.BaudRate = CBR_9600;  // Setting BaudRate = 9600
		dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
		dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
		dcbSerialParams.Parity = NOPARITY;  // Setting Parity = None

		SetCommState(hComm, &dcbSerialParams);

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50; // in milliseconds
		timeouts.ReadTotalTimeoutConstant = 50; // in milliseconds
		timeouts.ReadTotalTimeoutMultiplier = 10; // in milliseconds
		timeouts.WriteTotalTimeoutConstant = 50; // in milliseconds
		timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds

		return true;
	}
}

void handler(int s)
{
	signal(SIGINT, handler);
	// Close HANDLE Comm
	gpxFileManager.endFileStream();
	txtFileManager.endFileStream();
	CloseHandle(hComm);

	cout << "Closed in handler" << endl;

	exit(0);
}

