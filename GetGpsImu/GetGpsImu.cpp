// GetGpsImu.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define FILENAME "gps_log2"
#define GPSCOM L"COM8"
#define IMUCOM L"COM12"

using namespace std;

string* StringSplit(string strTarget, string strTok)
{
	int     nCutPos;
	int     nIndex = 0;
	string* strResult = new string[15];

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


bool openImuPort(wchar_t str[])
{
	//wchar_t str[1000] = PORTNUM;
	if (imuSerial.OpenPort(str)) {
		imuSerial.ConfigurePortW(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT);
		printf("IMU open sucessful");
		imuSerial.SetCommunicationTimeouts(0, 0, 0, 0, 0);

		imuSerial.WriteByte('s');
		imuSerial.WriteByte('p');
		imuSerial.WriteByte('=');
		imuSerial.WriteByte('5');
		imuSerial.WriteByte('0');
		imuSerial.WriteByte('0');
		//imuSerial.WriteByte('0');
		imuSerial.WriteByte('\n');

		imuSerial.WriteByte('s');
		imuSerial.WriteByte('s');
		imuSerial.WriteByte('=');
		imuSerial.WriteByte('7');
		imuSerial.WriteByte('\n');

		return true;
	}
	else {
		printf("IMU Connect failed\n");
		return false;
	}
}

bool openGpsPort(wchar_t str[])
{
	//wchar_t str[1000] = PORTNUM;

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
	// Close Files
	gpxFileManager.endFileStream();
	txtFileManager.endFileStream();
	imuFileStream.close();
	// Close Serials
	CloseHandle(hComm);
	imuSerial.ClosePort();

	cout << "Closed in handler" << endl;

	exit(0);
}

///////////////////// 전역 데이터 //////////////////////
GpsData g_gpsData;

///////////////////// 파일 출력 ////////////////////////
GpxFileManager gpxFileManager;
TxtFileManager txtFileManager;
ofstream imuFileStream;

///////////////////// Serial Port //////////////////////////
HANDLE hComm; // GPS Serial
CSerialPort imuSerial; // IMU Serial

int main()
{
	string filename = FILENAME;
	gpxFileManager.openFileStream(filename);
	txtFileManager.openFileStream(filename);
	imuFileStream.open("imu_log.txt");

	if (!openGpsPort(GPSCOM))
		return 1;
	if (!openImuPort(IMUCOM))
		return 1;

	signal(SIGINT, handler);

	DWORD NoBytesReadGps;
	bool check_tmp = false;

	BYTE* pByte = new BYTE[512];

	double Tdata;
	char Temp[8];
	double imu[9];

	char TempChar; //Temporary character used for reading
	char SerialBuffer[256]; //Buffer for storing Rxed Data
	DWORD NoBytesReadIMU;

	///////////////////////////////////////////////////////////////////

	while (true) {
		Ublox gps;
		char line[256];
		char data;
		int i = 0;

		line[0] = 1;

		while (true) {
			//////////////////////GPS ReadFile/////////////////////////
			ReadFile(hComm,           //Handle of the Serial port
				&data,       //Temporary character
				sizeof(data),//Size of TempChar
				&NoBytesReadGps,    //Number of bytes read
				NULL);

			if (NoBytesReadGps > 0)
				gps.encode(data); // 한 라인을 입력받으면 gps 변수 값이 없데이트된다.


			//////////////////////IMU ReadFile/////////////////////////
			ReadFile(imuSerial.m_hComm, &TempChar, sizeof(TempChar), &NoBytesReadIMU, NULL);
			imuFileStream << TempChar;

			SerialBuffer[i] = TempChar; //IMU Tempchar


			//////////////////////GPS EndLine/////////////////////////
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

					
					g_gpsData.setData(datetime, latitude, longitude, fixtype, satelliteInUse, hdop, altitude);
					gpxFileManager.writeGpsData(g_gpsData);
					txtFileManager.writeGpsData(g_gpsData);
				}
				break;
			}

			//////////////////////IMU EndLine/////////////////////////
			if (TempChar == '\n')
			{
				if (SerialBuffer[0] == 's');
				else
				{
					SerialBuffer[i + 1] = 0;
					puts(SerialBuffer);
				}
				//	printf("\n");
				i = 0;
			}
			if (TempChar != '\n')
			{
				i++;
			}
		}
	}

	// Close HANDLE Comm
	CloseHandle(hComm);

	return 0;
}
