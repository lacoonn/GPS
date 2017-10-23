// GetGpsImu.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
// 시리얼 포트로부터 GPS와 IMU 값을 읽고 파싱하는 프로젝트입니다.

#include "stdafx.h"
#include "GetGpsImu.h"
#include "ublox.h"
#include "Serial.h"

#define FILENAME "test_track4"
#define GPSCOM L"COM8"
#define IMUCOM L"COM4"

using namespace std;

///////////////////// 전역 데이터 //////////////////////
GpsData g_gpsData;

///////////////////// 뮤텍스 //////////////////////
mutex gps_mutex;
mutex imu_mutex;

///////////////////// 파일 출력 ////////////////////////
GpxFileManager gpxFileManager;
TxtFileManager txtFileManager;
ofstream imuFileStream;

///////////////////// Serial Port //////////////////////////
HANDLE hComm; // GPS Serial
CSerialPort imuSerial; // IMU Serial

///////////////////// 스레드 종료 판단 //////////////////////////
bool isFinish;

void handler(int s);
bool openGpsPort(wchar_t str[]);
bool openImuPort(wchar_t str[]);
void updateGps();
void updateImu();

int main()
{
	isFinish = false;
	
	string filename = FILENAME;
	
	// signal to terminate program
	signal(SIGINT, handler);

	if (!openGpsPort(GPSCOM))
		return 1;
	//if (!openImuPort(IMUCOM))
		//return 1;

	gpxFileManager.openFileStream(filename);
	txtFileManager.openFileStream(filename);
	string imufile = "imu_";
	imufile.append(FILENAME);
	imufile.append(".txt");
	imuFileStream.open(imufile);

	////////////////////////////// Thread Start /////////////////////////////////////
	thread gpsThread(updateGps);
	//thread imuThread(updateImu);

	////////////////////////////// Thread Join /////////////////////////////////////
	gpsThread.join();
	//imuThread.join();

	cout << "Exit Program" << endl;

	return 0;
}

void handler(int s)
{
	signal(SIGINT, handler);

	// End Thread
	isFinish = true;

	

	cout << "Closed in handler" << endl;

	CloseHandle(hComm);
}

bool openGpsPort(wchar_t str[])
{
	hComm = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	cout << "gps str : " << str << endl;



	if (hComm == INVALID_HANDLE_VALUE) {
		//printf("Error in opening serial port\n");
		cout << "GPS : Error in opening serial port" << endl;

		return false;
	}
	else {
		//printf("opening serial port successful\n");
		cout << "GPS : Open successful" << endl;

		DCB dcbSerialParams = { 0 }; // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		bool Status = GetCommState(hComm, &dcbSerialParams);

		dcbSerialParams.BaudRate = CBR_9600;  // Setting BaudRate = 9600
		dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
		dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
		dcbSerialParams.Parity = NOPARITY;  // Setting Parity = None
		dcbSerialParams.fParity = FALSE;

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

bool openImuPort(wchar_t str[])
{
	cout << "imu str : " << str << endl;
	//wchar_t str[1000] = PORTNUM;
	if (imuSerial.OpenPort(str)) {
		imuSerial.ConfigurePortW(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT);
		printf("IMU : Open sucessful\n");
		imuSerial.SetCommunicationTimeouts(0, 0, 0, 0, 0);

		imuSerial.WriteByte('s');
		imuSerial.WriteByte('p');
		imuSerial.WriteByte('=');
		imuSerial.WriteByte('3');
		imuSerial.WriteByte('0');
		//imuSerial.WriteByte('0');
		//imuSerial.WriteByte('0');
		imuSerial.WriteByte('\n');

		imuSerial.WriteByte('s');
		imuSerial.WriteByte('s');
		imuSerial.WriteByte('=');
		imuSerial.WriteByte('1');
		imuSerial.WriteByte('1');
		//imuSerial.WriteByte('7');
		imuSerial.WriteByte('\n');

		return true;
	}
	else {
		printf("IMU : Connect failed\n");
		return false;
	}
}

void updateGps()
{
	Ublox gps;
	GpsData tempGpsData;
	DWORD NumBytesReadGps;
	char data;

	SYSTEMTIME systemTime;

	while (true)
	{
		// 스레드가 종료되어야하는지 검사
		if (isFinish == true)
		{
			
			gpxFileManager.endFileStream();
			txtFileManager.endFileStream();
			cout << "updateGps thread end" << endl;
			return;
		}

		////////////////////// GPS ReadFile /////////////////////////
		ReadFile(hComm,           //Handle of the Serial port
			&data,       //Temporary character
			sizeof(data),//Size of data
			&NumBytesReadGps,    //Number of bytes read
			NULL);

		if (NumBytesReadGps > 0)
		{
			gps.encode(data); // 한 라인을 입력받으면 gps 변수 값이 없데이트된다.
							  //cout << data;
		}


		////////////////////// GPS EndLine /////////////////////////
		if (data == '\n') // 한 라인을 입력받으면 라인버퍼를 초기화한다.
		{
			if (gps.lastMessage == gps.GGA) // 최근 데이터가 GGA(유효한 데이터)일 경우
			{
				GetLocalTime(&systemTime);

				GpsData::time datetime;
				/*datetime.hours = (int)gps.datetime.hours;
				datetime.minutes = (int)gps.datetime.minutes;
				datetime.seconds = (int)gps.datetime.seconds;*/
				datetime.hours = (int)systemTime.wHour;
				datetime.minutes = (int)systemTime.wMinute;
				datetime.seconds = (int)systemTime.wSecond;
				datetime.milliseconds = (int)systemTime.wMilliseconds;
				double latitude = (double)gps.latitude;
				double longitude = (double)gps.longitude;
				int fixtype = (int)gps.fixtype;
				int satelliteInUse = (int)gps.sats_in_use;
				double hdop = (double)gps.hdop;
				double altitude = (double)gps.altitude;

				printf("GPS ==> %d:%d:%d:%d, %lf, %lf, %d, %d, %lf, %lf\n", datetime.hours, datetime.minutes, datetime.seconds, datetime.milliseconds, latitude, longitude, fixtype, satelliteInUse, hdop, altitude);

				tempGpsData.setData(datetime, latitude, longitude, fixtype, satelliteInUse, hdop, altitude);

				gps_mutex.lock();
				{
					g_gpsData = tempGpsData;
				}
				gps_mutex.unlock();

				gpxFileManager.writeGpsData(tempGpsData);
				txtFileManager.writeGpsData(tempGpsData);

				Sleep(100); // 버퍼 밀림을 방지
			}
		}
	}
}

void updateImu()
{
	BYTE ref;
	char TempChar; //Temporary character used for reading
	char SerialBuffer[256]; //Buffer for storing Rxed Data
	DWORD NumBytesReadIMU;
	int i = 0;

	while (true) {
		SYSTEMTIME systemTime;
		// 스레드가 종료되어야하는지 검사
		if (isFinish == true)
		{
			imuSerial.ClosePort();
			imuFileStream.close();
			cout << "updateImu thread end" << endl;
			return;
		}

		////////////////////// IMU ReadFile /////////////////////////
		//ReadFile(imuSerial.m_hComm, &TempChar, sizeof(TempChar), &NumBytesReadIMU, NULL);
		imuSerial.ReadByte(ref);
		TempChar = (char)ref;
		SerialBuffer[i] = TempChar;

		// Write TempChar to file
		//imuFileStream << TempChar;


		////////////////////// IMU EndLine /////////////////////////
		if (TempChar == '\n')
		{
			if (SerialBuffer[0] == 's');
			else
			{
				GetLocalTime(&systemTime);

				SerialBuffer[i + 1] = 0;
				//printf("IMU ==> Line End!\n");
				imuFileStream << (int)systemTime.wHour << " " << (int)systemTime.wMinute << " " << (int)systemTime.wSecond <<
					" " << (int)systemTime.wMilliseconds << " " << SerialBuffer;
			}
			//	printf("\n");
			i = 0;

			// IMU 데이터 업데이트 !!!!!!!!!!!!!!!!!!!!!!!!
			imu_mutex.lock();
			{

			}
			imu_mutex.unlock();
		}
		else
		{
			i++;
		}
	}
}