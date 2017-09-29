// ParseFromSerial.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
// Ubloxparser.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "ublox.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <csignal>
#include <vector>

#define GPXNAME "5E_outside"
#define PORTNUM L"COM8"

class GpsData
{
public:
	Ublox::_datetime datetime;
	float latitude; // 위도
	float longitude; // 경도
	int satelliteInUse;
	

	GpsData()
	{
		latitude = 0;
		longitude = 0;
		satelliteInUse = 0;
	}

	GpsData(Ublox::_datetime datetime_, float latitude_, float longitude_, int satelliteInUse_) {
		datetime = datetime_;
		latitude = latitude_; // 위도
		longitude = longitude_; // 경도
		satelliteInUse = satelliteInUse_;
	}

	void setData(Ublox::_datetime datetime_, float latitude_, float longitude_, int satelliteInUse_) {
		datetime = datetime_;
		latitude = latitude_; // 위도
		longitude = longitude_; // 경도
		satelliteInUse = satelliteInUse_;
	}
};


void GPS2GPX();
void handler(int signal);
bool openSerialPort();

using namespace std;


ofstream fs_out;
HANDLE hComm;
vector<GpsData> dataList;

int main()
{
	fs_out.open("gps_log.gpx");

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
					Ublox::_datetime datetime = gps.datetime;
					float latitude = gps.latitude;
					float longitude = gps.longitude;
					int satelliteInUse = (int)gps.sats_in_use;

					printf("%d:%d:%d, %f, %f, %d\n", (int)datetime.hours, (int)datetime.minutes, (int)datetime.seconds, latitude, longitude, satelliteInUse);

					GpsData tempData(datetime, latitude, longitude, satelliteInUse);
					dataList.push_back(tempData);
				}
				break;
			}
		}
	}

	GPS2GPX();
	
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

void handler(int signal)
{
	GPS2GPX();

	// Close HANDLE Comm
	CloseHandle(hComm);

	cout << "Closed in handler" << endl;

	exit(0);
}

void GPS2GPX()
{
	string gpxname = GPXNAME; // gpx 파일 xml <name>

	fs_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	fs_out << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xalan=\"http://xml.apache.org/xalan\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" creator=\"MotionX Live\" version=\"1.1\">" << endl;
	fs_out << "  <trk>" << endl;
	fs_out << "    <name>" << gpxname.c_str() << "</name>" << endl;
	fs_out << "    <desc>2016. 5. 11.  7:00 am</desc>" << endl;
	//(int)datetime.hours, (int)datetime.minutes, (int)datetime.seconds;
	
	fs_out << "    <trkseg>" << endl;

	int dataSize = dataList.size();
	for(int i = 0; i < dataSize; i++) {
		fs_out.precision(9);
		fs_out << "      <trkpt lat=\"" << dataList[i].latitude << "\" lon=\"" << dataList[i].longitude << "\">" << endl;
		fs_out << "        <ele></ele>" << endl;
		fs_out << "        <time></time>" << endl;
		fs_out << "      </trkpt>" << endl;
	}

	fs_out << "    </trkseg>" << endl;
	fs_out << "  </trk>" << endl;
	fs_out << "</gpx>" << endl;
}


/*
//
//Writing Data to Serial Port
//
char lpBuffer[] = "A";
DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port
dNoOFBytestoWrite = sizeof(lpBuffer);

Status = WriteFile(hComm,        // Handle to the Serial port
lpBuffer,     // Data to be written to the port
dNoOFBytestoWrite,  //No of bytes to write
&dNoOfBytesWritten, //Bytes written
NULL);
*/