// OptimalPath.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <climits>
#include <queue>
#include <vector>
#include <functional>

#define PI 3.141592653589793231

typedef struct Cdt {
	int row;
	int col;
};

class Vertex
{
public:
	int row;
	int col;
	float latitude; // 위도
	float longitude; // 경도
	Vertex *destination;
	
	Vertex()
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

typedef std::pair<Vertex, int> Item;
bool operator < (Item a, Item b) {
	return a.second < b.second;
}

std::ifstream fin;
Vertex corners[20][20];
std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;


int row, col;

void initMap();
void getNearest(Vertex point, Cdt &tgt1, Cdt &tgt2);
double getDistance(Vertex point1, Vertex point2);
void ShortestPath(Vertex graph[][20], Cdt src, Cdt tgt1, Cdt tgt2);
double deg2rad(double deg);
double rad2deg(double rad);

int main()
{
	fin.open("map.txt");
	initMap(); // 그래프를 초기화한다.
	Vertex *destination = new Vertex();
	fin >> destination->latitude >> destination->longitude;
	Cdt src = { 0, 0 };
	Cdt tgt1, tgt2;
	getNearest(*destination, tgt1, tgt2); // 주차지점을 입력받고 거기서 가장 가까운 vertex를 찾는다.
	corners[tgt1.row][tgt1.col].destination = destination; // tgt들의 destination변수에 destination vertex의 주소를 링크한다.
	corners[tgt2.row][tgt2.col].destination = destination;
	// 시작지점에서 어디 순으로 갈지 정한다.
	std::cout << "tgt1 : " << tgt1.row << " " << tgt1.col << std::endl;
	std::cout << "tgt2 : " << tgt2.row << " " << tgt2.col << std::endl;
	ShortestPath(corners, src, tgt1, tgt2); // 각 지점 간의 최단경로를 구한다.
	// 입력된 최단경로를 출력한다.


	// 동적할당 해제
	delete(destination);

    return 0;
}

void initMap()
{
	fin >> row >> col;
	row++;
	col++;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			double tempLat, tempLon;
			fin >> tempLat >> tempLon;
			corners[i][j].setLocation((float)tempLat, (float)tempLon);
			corners[i][j].row = i;
			corners[i][j].col = j;
		}
	}
}

void getNearest(Vertex point, Cdt &tgt1, Cdt &tgt2) // 한 점이 주어지면 그래프에서 가장 그 점에 가까운 두 개의 vertex를 반환한다
{
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> tempPQ; // Item의 second가 작은 순으로 정렬하는 pq
	double minDis = -1;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			minDis = getDistance(point, corners[i][j]);
			Item tempItem(corners[i][j], minDis);
			tempPQ.push(tempItem);
		}
	}
	Vertex tempVer; // second가 작은 순으로 두 점을 얻는다.
	tempVer = tempPQ.top().first;
	tempPQ.pop();
	tgt1 = { tempVer.row, tempVer.col };
	tempVer = tempPQ.top().first;
	tempPQ.pop();
	tgt2 = { tempVer.row, tempVer.col };
}

double getDistance(Vertex point1, Vertex point2)
{
	// 두 점(위도, 경도) 사이의 거리를 구한다
	double lat1 = (double)point1.latitude;
	double lon1 = (double)point1.longitude;
	double lat2 = (double)point2.latitude;
	double lon2 = (double)point2.longitude;
	double theta = lon1 - lon2;
	double dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));


	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;

	// meter
	dist = dist * 1609.344;
	//centimeter
	//dist = dist * 160934.4;
	

	return dist;
}

// This function converts decimal degrees to radians
double deg2rad(double deg) {
	return (deg * PI / 180.0);
}

// This function converts radians to decimal degrees
double rad2deg(double rad) {
	return (rad * 180 / PI);
}

void ShortestPath(Vertex graph[][20], Cdt src, Cdt tgt1, Cdt tgt2) // Dijkstra 알고리즘을 사용해서 두 목적지 중 가까운 곳의 최단경로를 구한다.
{
	int dist[20][20];
	Cdt prev[20][20];
	Cdt ftgt; // 두 목적지 중 어디로 갔는지 저장하는 변수ㄴ

	dist[src.row][src.col] = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (i != src.row || j != src.col) {
				dist[i][j] = INT_MAX;
			}
			prev[i][j] = { -1, -1 };
			Item item(graph[i][j], dist[i][j]);
			pq.push(item);
		}
	}
	
	
	while(pq.empty() != true)
	{
		Vertex u = pq.top().first;
		pq.pop();
		int uRow = u.row;
		int uCol = u.col;

		if (uRow == tgt1.row && uCol == tgt1.col) {
			ftgt = tgt1;// 어디로 갔는지 표시하는 코드 추가
			break;
		}
		if (uRow == tgt2.row && uCol == tgt2.col) {
			ftgt = tgt2;// 어디로 갔는지 표시하는 코드 추가
			break;
		}

		if (uRow > 0) { // up
			Vertex v = corners[uRow - 1][uCol];

			int tempDist = dist[uRow][uCol] + 1;

			if (tempDist < dist[v.row][v.col]) {
				dist[v.row][v.col] = tempDist;
				prev[v.row][v.col] = { u.row, u.col };
				Item item(v, tempDist);
				pq.push(item);
			}
		}
		if (uRow < row - 1) { // down
			Vertex v = corners[uRow + 1][uCol];

			int tempDist = dist[uRow][uCol] + 1;

			if (tempDist < dist[v.row][v.col]) {
				dist[v.row][v.col] = tempDist;
				prev[v.row][v.col] = { u.row, u.col };
				Item item(v, tempDist);
				pq.push(item);
			}
		}
		if (uCol > 0) { // left
			Vertex v = corners[uRow][uCol - 1];

			int tempDist = dist[uRow][uCol] + 1;

			if (tempDist < dist[v.row][v.col]) {
				dist[v.row][v.col] = tempDist;
				prev[v.row][v.col] = { u.row, u.col };
				Item item(v, tempDist);
				pq.push(item);
			}
		}
		if (uCol < col - 1) { // right
			Vertex v = corners[uRow][uCol + 1];

			int tempDist = dist[uRow][uCol] + 1;

			if (tempDist < dist[v.row][v.col]) {
				dist[v.row][v.col] = tempDist;
				prev[v.row][v.col] = { u.row, u.col };
				Item item(v, tempDist);
				pq.push(item);
			}
		}
	}
	/*
	1  S ← empty sequence
	2  u ← target
	3  while prev[u] is defined:                  // Construct the shortest path with a stack S
	4      insert u at the beginning of S         // Push the vertex onto the stack
	5      u ← prev[u]                            // Traverse from target to source
	6  insert u at the beginning of S             // Push the source onto the stack
	*/
	std::vector<Cdt> S;
	Cdt u = ftgt;
	while (prev[u.row][u.col].row != -1 && prev[u.row][u.col].col != -1) {
		S.push_back(u);
		u = { prev[u.row][u.col].row, prev[u.row][u.col].col };
	}
	S.push_back(src);

	// Print Path
	while (!S.empty()) {
		Cdt temp = S.back();
		S.pop_back();
		std::cout << temp.row << " " << temp.col << std::endl;
	}
	std::cout << "\n\n\n";
}
