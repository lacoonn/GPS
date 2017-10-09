/*
Andrea Toscano
U-BLOX NEO M8M Parser
*/

#include <stdio.h>
#include <tchar.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


class Ublox
{
public:

	class Tokeniser
	{
	public:
		Tokeniser(char* _str, char _token)
		{
			str = _str;
			token = _token;
		}

		bool next(char* out, int len)
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

	private:
		char* str;
		char token;
	};


	struct satellite
	{
		uint8_t prn;
		int16_t elevation;
		int16_t azimuth;
		uint8_t snr; //signal to noise ratio
	};

	struct _datetime
	{
		uint8_t day, month, year;
		uint8_t hours, minutes, seconds;
		uint16_t millis;
		bool valid; //1 = yes, 0 = no
	};

	enum _fix { FIX_NONE = 1, FIX_2D, FIX_3D };
	enum _op_mode { MODE_MANUAL, MODE_AUTOMATIC };
	enum NEMA { GGA, GSA, GSV, RMC, VTG };


	bool encode(char c)
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


	NEMA lastMessage;
	//float latitude, longitude;
	double latitude, longitude;
	float altitude, vert_speed;
	int latlng_age, alt_age;

	//these units are in hundredths
	//so a speed of 5260 means 52.60km/h
	uint16_t speed, course, knots;
	int speed_age, course_age, knots_age;

	int fixtype; //0 = no fix, 1 = satellite only, 2 = differential fix
	int fixtype_age;
	_fix fix;
	int fix_age;

	float pdop, hdop, vdop; //positional, horizontal and vertical dilution of precision
	int dop_age;

	int8_t sats_in_use;
	int8_t sats_in_view;

	satellite sats[12];
	int sats_age;

	_datetime datetime;
	int time_age, date_age;

	_op_mode op_mode;

private:
	char buf[120];
	uint8_t pos;

	bool check_checksum();

	uint8_t parse_hex(char c)
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

	bool process_buf()
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

	// GNGGA 
	void read_gga()
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
	void read_gsa();
	void read_gsv();
	void read_rmc();
	void read_vtg();

};



//extern Ublox gps;


// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
