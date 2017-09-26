// CreatingMap.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

using namespace std;


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


    return 0;
}

