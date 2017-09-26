#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <thread>
#include <fstream>
#include <sstream>
#include <nmea/nmea.h>

//#include "GPSDevice.h"

using namespace std;

bool debugverbose = true;
void debug(string message) {
	if (debugverbose)
		cout << "[ DEBUG ] " << message << endl;
}

bool errorverbose = true;
void error(string message) {
	if (errorverbose)
		cout << "[ ERROR ] " << message << endl;
	exit(-1);
}

//GPSDevice gpsDevice;

nmeaINFO nmeainfo;
nmeaPARSER nmeaparser;

nmeaPOS nmeapos;

void nmeatrace(const char *str, int str_size) {
	//printf("[ NMEA TRACE ] ");
	//write(1, str, str_size);
	//printf("\n");
}

void nmeaerror(const char *str, int str_size) {
	//printf("[ NMEA ERROR ] ");
	//write(1, str, str_size);
	//printf("\n");
}

int main(int argc, char *argv[]) {

	ifstream file_in("log_code.txt");


	// open gps device
	debug("opening gps device");
	//GPSDevice gpsDevice("/dev/ttyO3");
	//if (!(gpsDevice.openport()))
		//error("gpsDevice.openport()");

	// set up nmea parser
	debug("setting up nmea parser");
	nmea_property()->trace_func = &nmeatrace;
	nmea_property()->error_func = &nmeaerror;

	nmea_zero_INFO(&nmeainfo);
	nmea_parser_init(&nmeaparser);

	// read nmea data
	debug("reading nmea data");
	for (;;) {
		//string data = gpsDevice.readline();
		string data;
		char line[256];

		file_in.getline(line, sizeof(line));
		cout << line[2] << endl;
		if (line[2] == 'N')
			line[2] = 'P';
		data = line;


		/*
		// test part
		if (line[2] == 'N')
			line[2] = 'P';
		cout << line << endl;
		//test part END
		*/


		debug(data);

		// nmea_parser expects \r\n
		data += "\r\n";

		// parse nmea data
		nmea_parse(&nmeaparser, data.c_str(), (int)strlen(data.c_str()), &nmeainfo);

		// get position
		nmea_info2pos(&nmeainfo, &nmeapos);

		cout << "Lat: " << nmeapos.lat << endl;
		cout << "Lon: " << nmeapos.lon << endl;
	}

	// destroy parser
	debug("destroying parser");
	nmea_parser_destroy(&nmeaparser);

	// close gps device
	debug("closing gps device");
	//gpsDevice.closeport();

	return 0;
}