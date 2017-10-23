/*
Andrea Toscano
U-BLOX NEO M8M Parser
*/

#pragma once

#include "stdafx.h"

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

		bool next(char* out, int len);

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

	bool encode(char c);

private:
	char buf[120];
	uint8_t pos;

	uint8_t parse_hex(char c);

	bool process_buf();

	// GNGGA 
	void read_gga();
};



//extern Ublox gps;


// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
