// ParseFromSerial.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
// Ubloxparser.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "ublox.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

#define ITR 100
#define GPXNAME "5E_outside"

class Point
{
public:
	float latitude; // 위도
	float longitude; // 경도
	Point *destination;

	Point()
	{
		latitude = 0;
		longitude = 0;
		destination = NULL;
	}

	void setLocation(float lat, float lon) {
		latitude = lat;
		longitude = lon;
	}
};

void GPS2GPX();

using namespace std;

string gpxname = GPXNAME; // gpx 파일 xml <name>
ofstream fs_out;
int gcnt;
Point pointList[1000];

int main()
{
	HANDLE hComm;
	wchar_t str[1000] = L"COM7";
	
	fs_out.open("gps_log.gpx");

	hComm = CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hComm == INVALID_HANDLE_VALUE)
		printf("Error in opening serial port\n");
	else
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

											   //
											   // Reading from the Serial Port
											   //
	DWORD NoBytesRead;
	int i = 0;
	

	bool check_tmp = false;
	for (;;) {
		char line[256];
		char data;
		Ublox gps;

		line[0] = 1;

		//file_in.getline(line, sizeof(line));
		


		while (1) {
			ReadFile(hComm,           //Handle of the Serial port
				&data,       //Temporary character
				sizeof(data),//Size of TempChar
				&NoBytesRead,    //Number of bytes read
				NULL);

			if (NoBytesRead > 0)
				gps.encode(data);

			if (data == '\n') {
				break;
			}
		}
		if (gcnt == ITR)
			break;
		
	}

	GPS2GPX();
	
	// Close HANDLE Comm
	CloseHandle(hComm);

	return 0;
}

Ublox::Tokeniser::Tokeniser(char* _str, char _token)
{
	str = _str;
	token = _token;
}


bool Ublox::Tokeniser::next(char* out, int len)
{
	uint8_t count = 0;

	if (str[0] == 0)
		return false;

	while (true)
	{
		if (str[count] == '\0')
		{
			out[count] = '\0';
			str = &str[count];
			return true;
		}

		if (str[count] == token)
		{
			out[count] = '\0';
			count++;
			str = &str[count];
			return true;
		}

		if (count < len)
			out[count] = str[count];

		count++;
	}
	return false;
}


bool Ublox::encode(char c)
{
	buf[pos] = c;
	pos++;

	if (c == '\n') //linefeed
	{
		//cout << buf << endl;
		bool ret = process_buf();
		memset(buf, '\0', 120);
		pos = 0;
		return ret;
	}

	if (pos >= 120) //avoid a buffer overrun
	{
		memset(buf, '\0', 120);
		pos = 0;
	}
	return false;
}


bool Ublox::process_buf()
{
	/*
	if (!check_checksum()) //if checksum is bad
	{
	return false; //return
	}
	*/

	//otherwise, what sort of message is it
	if (strncmp(buf, "$GNGGA", 6) == 0)

	{
		read_gga();
	}
	if (strncmp(buf, "$GNGSA", 6) == 0)
	{
		//read_gsa();
	}

	if (strncmp(buf, "$GPGSV", 6) == 0)
	{
		//read_gsv();
	}

	if (strncmp(buf, "$GNRMC", 6) == 0)

	{
		//read_rmc();
	}
	if (strncmp(buf, "$GNVTG", 6) == 0)
	{
		//read_vtg();
	}
	return true;
}

// GNGGA 
void Ublox::read_gga()
{
	int counter = 0;
	char token[20];
	Tokeniser tok(buf, ',');

	while (tok.next(token, 20))
	{
		switch (counter)
		{
		case 1: //time
		{
			float time = atof(token);
			int hms = int(time);

			datetime.millis = time - hms;
			datetime.seconds = fmod(hms, 100);
			hms /= 100;
			datetime.minutes = fmod(hms, 100);
			hms /= 100;
			datetime.hours = hms;

			//time_age = millis();
		}
		break;
		case 2: //latitude
		{
			float llat = atof(token);
			int ilat = llat / 100;
			double mins = fmod(llat, 100);
			latitude = ilat + (mins / 60);
		}
		break;
		case 3: //north/south
		{
			if (token[0] == 'S')
				latitude = -latitude;
		}
		break;
		case 4: //longitude
		{
			float llong = atof(token);
			int ilat = llong / 100;
			double mins = fmod(llong, 100);
			longitude = ilat + (mins / 60);
		}
		break;
		case 5: //east/west
		{
			if (token[0] == 'W')
				longitude = -longitude;
			//latlng_age = millis();
		}
		break;
		case 6:
		{
			fixtype = _fixtype(atoi(token));
		}
		break;
		case 7:
		{
			sats_in_use = atoi(token);
		}
		break;
		case 8:
		{
			hdop = atoi(token);
		}
		break;
		case 9:
		{
			float new_alt = atof(token);
			//vert_speed = (new_alt - altitude) / ((millis() - alt_age) / 1000.0);
			altitude = atof(token);
			//alt_age = millis();
		}
		break;
		}
		counter++;
	}
	//cout << latitude << " " << longitude << endl;
	printf("%d : %f %f\n", gcnt, latitude, longitude);
	//fs_out.precision(12);
	//fs_out << latitude << " " << longitude << endl;
	
	pointList[gcnt].setLocation(latitude, longitude);
	gcnt++;
}


bool Ublox::check_checksum()
{
	if (buf[strlen(buf) - 5] == '*')
	{
		uint16_t sum = parse_hex(buf[strlen(buf) - 4]) * 16;
		sum += parse_hex(buf[strlen(buf) - 3]);

		for (uint8_t i = 1; i < (strlen(buf) - 5); i++)
			sum ^= buf[i];
		if (sum != 0)
			return false;

		return true;
	}
	return false;
}


uint8_t Ublox::parse_hex(char c)
{
	if (c < '0')
		return 0;
	if (c <= '9')
		return c - '0';
	if (c < 'A')
		return 0;
	if (c <= 'F')
		return (c - 'A') + 10;
	return 0;
}

void GPS2GPX()
{
	fs_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	fs_out << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xalan=\"http://xml.apache.org/xalan\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" creator=\"MotionX Live\" version=\"1.1\">" << endl;
	fs_out << "  <trk>" << endl;
	fs_out << "    <name>" << gpxname.c_str() << "</name>" << endl;
	fs_out << "    <desc>2016. 5. 11.  7:00 am</desc>" << endl;
	fs_out << "    <trkseg>" << endl;


	for(int i = 0; i < ITR; i++) {
		fs_out.precision(12);
		fs_out << "      <trkpt lat=\"" << pointList[i].latitude << "\" lon=\"" << pointList[i].longitude << "\">" << endl;
		fs_out << "        <ele></ele>" << endl;
		fs_out << "        <time></time>" << endl;
		fs_out << "      </trkpt>" << endl;
	}

	fs_out << "    </trkseg>" << endl;
	fs_out << "  </trk>" << endl;
	fs_out << "</gpx>" << endl;
}