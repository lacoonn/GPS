#include "stdafx.h"
#include "ublox.h"

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

bool Ublox::process_buf()
{
	//otherwise, what sort of message is it
	if (strncmp(buf, "$GNGGA", 6) == 0)
	{
		lastMessage = GGA;
		read_gga();
	}
	if (strncmp(buf, "$GNGSA", 6) == 0)
	{
		lastMessage = GSA;
		//read_gsa();
	}

	if (strncmp(buf, "$GPGSV", 6) == 0)
	{
		lastMessage = GSV;
		//read_gsv();
	}

	if (strncmp(buf, "$GNRMC", 6) == 0)
	{
		lastMessage = RMC;
		//read_rmc();
	}
	if (strncmp(buf, "$GNVTG", 6) == 0)
	{
		lastMessage = VTG;
		//read_vtg();
	}
	return true;
}

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

			datetime.hours += 9; // 한국 시간 보정

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
			//float llong = atof(token);
			double llong = (double)(atof(token));
			int ilat = llong / 100;
			double mins = fmod(llong, 100.0);
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
			fixtype = (int)atoi(token); // 0 : invalid, 1 : gps, 2 : dgps
		}
		break;
		case 7:
		{
			sats_in_use = atoi(token); // 사용 중인 인공위성 숫자
		}
		break;
		case 8:
		{
			hdop = atoi(token); // horizontal dilution of Precision으로 2차원적 오차결정(수평방향)을 뜻한다.
		}
		break;
		case 9:
		{
			float new_alt = atof(token);
			//vert_speed = (new_alt - altitude) / ((millis() - alt_age) / 1000.0);
			altitude = atof(token); // 해수면 기준 고도
									//alt_age = millis();
		}
		break;
		}
		counter++;
	}
}