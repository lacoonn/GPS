// CreatingMap.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
// 테스트를 위해 Global Path Planning의 맵을 생성해보기 위한 프로젝트입니다.

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <climits>

using namespace std;

void GPS2GPX(int row, int col)
{
	ofstream fs_out("map.gpx");
	string gpxname = "TEST SET";
	float lat = 35.8869094849;
	float lon = 128.611282349;

	fs_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	fs_out << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:xalan=\"http://xml.apache.org/xalan\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" creator=\"MotionX Live\" version=\"1.1\">" << endl;
	fs_out << "  <trk>" << endl;
	fs_out << "    <name>" << gpxname.c_str() << "</name>" << endl;
	fs_out << "    <desc>2017. 9. 26.  7:00 am</desc>" << endl;
	fs_out << "    <trkseg>" << endl;


	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			fs_out.precision(12);
			fs_out << "      <trkpt lat=\"" << lat + 0.0001 * i << "\" lon=\"" << lon + 0.0001 * j << "\">" << endl;
			fs_out << "        <ele></ele>" << endl;
			fs_out << "        <time></time>" << endl;
			fs_out << "      </trkpt>" << endl;
		}
	}

	fs_out << "    </trkseg>" << endl;
	fs_out << "  </trk>" << endl;
	fs_out << "</gpx>" << endl;
}

int main()
{
	ofstream fout("map.txt");
	"35.8869094849 128.611282349";
	float lat = 35.8869094849;
	float lon = 128.611282349;

	int row = 3;
	int col = 5;
	fout << row << " " << col << endl;
	fout.precision(9);
	for (int i = 0; i < row + 1; i++) {
		for (int j = 0; j < col + 1; j++) {
			fout << lat + 0.0001 * i << " " << lon + 0.0001 * j << " ";
		}
		fout << endl;
	}
	// gps는 소숫점 4번째 자리부터 유의미한 차이가 보임
	GPS2GPX(row + 1, col + 1);

	double tempDouble = DBL_MAX;
	printf("%lf\n", tempDouble);
	int tempInt = INT_MAX;
	printf("%d\n", tempInt);
    return 0;
}

