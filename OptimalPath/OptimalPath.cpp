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
Vertex *getNearest(Vertex point);
double getDistance(Vertex point1, Vertex point2);
void ShortestPath(Vertex graph[][20], Cdt src, Cdt tgt);
double deg2rad(double deg);
double rad2deg(double rad);

int main()
{
	fin.open("map.txt");
	initMap(); // 그래프를 초기화한다.
	Vertex destination;
	fin >> destination.latitude >> destination.longitude;
	Vertex *target = getNearest(destination); // 주차지점을 입력받고 거기서 가장 가까운 vertex를 찾는다.
	// 시작지점에서 어디 순으로 갈지 정한다.
	Cdt src = { 0, 0 };
	Cdt tgt = { target->row, target->col };
	ShortestPath(corners, src, tgt); // 각 지점 간의 최단경로를 구한다.
	// 입력된 최단경로를 출력한다.

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
	std::cout << "check" << std::endl;
}

Vertex *getNearest(Vertex point) // 한 점이 주어지면 그래프에서 가장 그 점에 가까운 vertex를 반환한다
{
	Vertex *tempVertex = NULL;
	double minDis = -1;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (minDis < 0) {
				minDis = getDistance(point, corners[i][j]);
				tempVertex = &corners[i][j];
			}
			else {
				double tempDis = getDistance(point, corners[i][j]);
				if (tempDis < minDis) {
					minDis = tempDis;
					tempVertex = &corners[i][j];
				}
			}
		}
	}

	return tempVertex;
}

double getDistance(Vertex point1, Vertex point2)
{
	// 두 점(위도, 경도) 사이의 거리를 구한다
	//float theta = point1.longitude - point2.longitude;
	//double dist = sin(deg2rad(point1.latitude)) * sin(deg2rad(point2.latitude)) + cos(deg2rad(point1.latitude)) * cos(deg2rad(point2.latitude)) * cos(deg2rad(theta));
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

void ShortestPath(Vertex graph[][20], Cdt src, Cdt tgt) // Dijkstra 알고리즘을 사용해서 두 점 사이의 최단거리를 구한다.
{
	int dist[20][20];
	Cdt prev[20][20];

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

		if (uRow == tgt.row && uCol == tgt.col)
			break;

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
	Cdt u = tgt;
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
}
