// Ubloxparser.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	ifstream file_in("log_code.txt");


	for (;;) {
		char line[256];
		char data;
		Ublox gps;

		line[0] = 1;


		//file_in.getline(line, sizeof(line));
		while (1) {
			data = file_in.get();
			//file_in >> data;
			//cout << data;
			if (file_in.eof())
				return 0;

			
			gps.encode(data);
			if (data == '\n') {
				break;
			}
		}


		// parse nmea data

		//cout << gps.latitude << " " << gps.longitude<< endl;
	}

	//gpsDevice.closeport();

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
		cout << buf << endl;
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
	cout << latitude << " " << longitude << endl;
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


