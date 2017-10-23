#pragma once

class GpsData
{
public:
	struct time
	{
		int hours, minutes, seconds, milliseconds;
	};
	time datetime;
	double latitude; // ����
	double longitude; // �浵
	int fixtype; // 0 = invalid, 1 = gps, 2 = dgps
	int satelliteInUse;
	double hdop;
	double altitude;


	GpsData()
	{
		latitude = 0;
		longitude = 0;
		satelliteInUse = 0;
	}

	void setData(time datetime_, double latitude_, double longitude_, int fixtype_, int satelliteInUse_, double hdop_, double altitude_) // �Է� ������ Ŭ������ ���ǵ� ���� �������!
	{
		datetime = datetime_;
		latitude = latitude_; // ����
		longitude = longitude_; // �浵
		fixtype = fixtype_;
		satelliteInUse = satelliteInUse_;
		hdop = hdop_;
		altitude = altitude_;
	}
};

class GpxFileManager
{
public:
	std::ofstream gpxFileStream;

	GpxFileManager()
	{

	}

	~GpxFileManager()
	{
		if (gpxFileStream.is_open())
		{
			gpxFileStream << "    </trkseg>" << std::endl;
			gpxFileStream << "  </trk>" << std::endl;
			gpxFileStream << "</gpx>" << std::endl;

			gpxFileStream.close();
		}
	}

	void openFileStream(std::string _filename)
	{
		std::string tempString = "gps_";
		tempString.append(_filename);
		tempString.append(".gpx");
		gpxFileStream.open(tempString);

		std::string gpxname = _filename; // gpx ���� xml <name>

		gpxFileStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
		gpxFileStream << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xalan=\"http://xml.apache.org/xalan\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" creator=\"MotionX Live\" version=\"1.1\">" << std::endl;
		gpxFileStream << "  <trk>" << std::endl;
		gpxFileStream << "    <name>" << gpxname.c_str() << "</name>" << std::endl;
		gpxFileStream << "    <desc>2016. 5. 11.  7:00 am</desc>" << std::endl;
		//(int)datetime.hours, (int)datetime.minutes, (int)datetime.seconds;

		gpxFileStream << "    <trkseg>" << std::endl;
	}


	void writeGpsData(GpsData data)
	{
		gpxFileStream.precision(9);
		gpxFileStream << "      <trkpt lat=\"" << data.latitude << "\" lon=\"" << data.longitude << "\">" << std::endl;
		gpxFileStream << "        <ele></ele>" << std::endl;
		gpxFileStream << "        <time></time>" << std::endl;
		gpxFileStream << "      </trkpt>" << std::endl;
	}

	void endFileStream()
	{
		gpxFileStream << "    </trkseg>" << std::endl;
		gpxFileStream << "  </trk>" << std::endl;
		gpxFileStream << "</gpx>" << std::endl;

		gpxFileStream.close();
	}
};

class TxtFileManager
{
public:
	std::ofstream txtFileStream;

	TxtFileManager()
	{

	}

	~TxtFileManager()
	{
		if (txtFileStream.is_open())
		{
			txtFileStream.close();
		}
	}

	void openFileStream(std::string _filename)
	{
		std::string tempString = "gps_";
		tempString.append(_filename);
		tempString.append(".txt");
		txtFileStream.open(tempString);
	}

	void writeGpsData(GpsData data)
	{
		txtFileStream << (int)data.datetime.hours << " " << (int)data.datetime.minutes << " " << (int)data.datetime.seconds << " " <<
			data.datetime.milliseconds << " " << data.latitude << " " << data.longitude << " " << data.fixtype << " " <<
			data.satelliteInUse << " " << data.hdop << " " << data.altitude << std::endl;
	}

	void endFileStream()
	{
		txtFileStream.close();
	}
};
