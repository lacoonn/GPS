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
	double latitude; // 위도
	double longitude; // 경도
	Vertex *destination;
	bool impossible;
	
	Vertex()
	{
		row = -1;
		col = -1;
		latitude = 0;
		longitude = 0;
		destination = NULL;
		impossible = false;
	}

	void setLocation(double lat, double lon) {
		latitude = lat;
		longitude = lon;
	}

	bool isImpossible() {
		return impossible;
	}
};

typedef std::pair<Vertex, int> Item;
bool operator < (Item a, Item b) {
	return a.second < b.second;
}

// Functions
void initMap();
Vertex *getNearestPointFromDesList(Cdt source, std::vector<Vertex *> &desList);
void getNearestTwoPoint(Vertex &point, Cdt &target1, Cdt &target2);
Cdt ShortestPath(Vertex graph[][20], Cdt src, Cdt target1, Cdt target2);
double deg2rad(double deg);
double rad2deg(double rad);
double getDistance(Vertex point1, Vertex point2);
double getBearing(Vertex point1, Vertex point2);


// Global Variables
std::ifstream fin;
Vertex corners[20][20];
int row, col;

int main()
{
	fin.open("map.txt"); // 현재 목적지들에 가장 가까운 점은 (0, 5), (3, 0), (3, 5) 이다.
	
	initMap(); // 그래프를 초기화한다.

	//방위각
	std::cout << "bearing --> [0][0] to [3][5] : " << getBearing(corners[0][0], corners[3][5]) << std::endl;
	std::cout << "distance --> [0][0] to [3][5] : " << getDistance(corners[0][0], corners[3][5]) << std::endl;
	//방위각
	
	int desNum; // 목적지들을 입력받고 vector에 저장한다.
	fin >> desNum;
	std::vector<Vertex *> desList;
	std::vector<Vertex *> desListForDelete;
	for (int i = 0; i < desNum; i++) {
		Vertex *destination = new Vertex();
		fin >> destination->latitude >> destination->longitude;
		desList.push_back(destination);
		desListForDelete.push_back(destination);
	}
	
	Cdt src = { 0, 0 }; // 일단 시작점은 (0, 0)으로 가정한다.

	for (int i = 0; i < desNum; i++) {
		Vertex *destination = getNearestPointFromDesList(src, desList); // 현재 위치에서 어떤 주차지점으로 먼저 갈지 정한다.
		
		Cdt target1, target2;
		getNearestTwoPoint(*destination, target1, target2); // 주차지점을 통해 거기서 가장 가까운 corner 두 개를 찾는다.
		corners[target1.row][target1.col].destination = destination; // target corner들의 destination변수에 destination vertex의 주소를 링크한다.
		corners[target2.row][target2.col].destination = destination;
		
		//std::cout << "target1 : " << target1.row << " " << target1.col << std::endl;
		//std::cout << "target2 : " << target2.row << " " << target2.col << std::endl;
		Cdt tempSrc = ShortestPath(corners, src, target1, target2); // 각 지점 간의 최단경로를 구한다.

		src = tempSrc; // 출발점을 업데이트한다.
		// 입력된 최단경로를 출력한다.(현재 ShortestPath 함수 안에 구현)
	}

	// 동적할당 해제
	while (!desListForDelete.empty()) {
		Vertex *tempDes = desListForDelete.back();
		desListForDelete.pop_back();
		delete(tempDes);
	}

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
			corners[i][j].setLocation(tempLat, tempLon);
			corners[i][j].row = i;
			corners[i][j].col = j;
		}
	}
}

Vertex *getNearestPointFromDesList(Cdt source, std::vector<Vertex *> &desList) // 한 점이 주어지면 desList에서 그 점에 가장 가까운 목적지를 구한다.
{
	Vertex *tempVertex = NULL;
	double minDis = DBL_MAX;
	int minIndex;
	int desSize = desList.size();
	
	for (int i = 0; i < desSize; i++) {
		double tempDis = getDistance(corners[source.row][source.col], *(desList.at(i)));
		if (minDis > tempDis) {
			minDis = tempDis;
			minIndex = i;
			tempVertex = desList.at(i);
		}
	}
	desList.erase(desList.begin() + minIndex);

	return tempVertex;

	/*for(int i = 0; i < desSize; i++) {
	if (minDis < 0) { // 첫번째 루프일 때
	minDis = getDistance(corners[source.row][source.col], *(desList.at(i)));
	minIndex = i;
	tempVertex = desList.at(i);
	}
	else {
	double tempDis = getDistance(corners[source.row][source.col], *(desList.at(i)));
	if (minDis > tempDis) {
	minDis = tempDis;
	minIndex = i;
	tempVertex = desList.at(i);
	}
	}
	}*/
}

void getNearestTwoPoint(Vertex &point, Cdt &target1, Cdt &target2) // 주차지점이 주어지면 거기서 가장 가까운 두 개의 코너를 반환한다
{
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> tempPQ; // Item의 second가 작은 순으로 정렬하는 pq
	double minDis = -1.0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			minDis = getDistance(point, corners[i][j]);
			Item tempItem(corners[i][j], minDis);
			tempPQ.push(tempItem);
		}
	}
	Vertex tempVer; // second가 작은 순으로 두 점을 얻는다.
	// 가장 가까운 코너
	tempVer = tempPQ.top().first;
	tempPQ.pop();
	corners[tempVer.row][tempVer.col].destination = &point; // 코너 노드의 destination에 주차지점을 링크
	target1 = { tempVer.row, tempVer.col };
	// 가장 먼 코너
	tempVer = tempPQ.top().first;
	tempPQ.pop();
	target2 = { tempVer.row, tempVer.col };
	corners[tempVer.row][tempVer.col].destination = &point; // 코너 노드의 destination에 주차지점을 링크
}

// This function converts decimal degrees to radians
double deg2rad(double deg) {
	return (deg * PI / 180.0);
}

// This function converts radians to decimal degrees
double rad2deg(double rad) {
	return (rad * 180 / PI);
}

Cdt ShortestPath(Vertex graph[][20], Cdt src, Cdt target1, Cdt target2) // Dijkstra 알고리즘을 사용해서 두 목적지 중 가까운 곳의 최단경로를 구한다.
{
	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
	int dist[20][20];
	Cdt prev[20][20];
	Cdt finalTarget; // 주차지점으로 향하기 위해 들러야 하는 마지막 corner
	Cdt nextSource; // 두 타겟지역 중 하나는 다음 경로를 위한 시작점이 된다.(주차지점이 두 타겟 사이에 있기 때문에)

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

		if (uRow == target1.row && uCol == target1.col) {
			finalTarget = target1;// 어디로 갔는지 표시하는 코드 추가
			corners[finalTarget.row][finalTarget.col].impossible = true; // finalTarget은 목표지에 도달한 후 곧바로 돌아갈 수는 없는 노드이다.
			nextSource = target2;
			break;
		}
		if (uRow == target2.row && uCol == target2.col) {
			finalTarget = target2;// 어디로 갔는지 표시하는 코드 추가
			corners[finalTarget.row][finalTarget.col].impossible = true; // finalTarget은 목표지에 도달한 후 곧바로 돌아갈 수는 없는 노드이다.
			nextSource = target1;
			break;
		}

		if (uRow > 0) { // up
			Vertex v = corners[uRow - 1][uCol];
			if (v.isImpossible()) {
				v.impossible = false;
			}
			else {
				int tempDist = dist[uRow][uCol] + 1;

				if (tempDist < dist[v.row][v.col]) {
					dist[v.row][v.col] = tempDist;
					prev[v.row][v.col] = { u.row, u.col };
					Item item(v, tempDist);
					pq.push(item);
				}
			}
		}
		if (uRow < row - 1) { // down
			Vertex v = corners[uRow + 1][uCol];
			if (v.isImpossible()) {
				v.impossible = false;
			}
			else {
				int tempDist = dist[uRow][uCol] + 1;

				if (tempDist < dist[v.row][v.col]) {
					dist[v.row][v.col] = tempDist;
					prev[v.row][v.col] = { u.row, u.col };
					Item item(v, tempDist);
					pq.push(item);
				}
			}
		}
		if (uCol > 0) { // left
			Vertex v = corners[uRow][uCol - 1];
			if (v.isImpossible()) {
				v.impossible = false;
			}
			else {
				int tempDist = dist[uRow][uCol] + 1;

				if (tempDist < dist[v.row][v.col]) {
					dist[v.row][v.col] = tempDist;
					prev[v.row][v.col] = { u.row, u.col };
					Item item(v, tempDist);
					pq.push(item);
				}
			}
		}
		if (uCol < col - 1) { // right
			Vertex v = corners[uRow][uCol + 1];
			if (v.isImpossible()) {
				v.impossible = false;
			}
			else {
				int tempDist = dist[uRow][uCol] + 1;

				if (tempDist < dist[v.row][v.col]) {
					dist[v.row][v.col] = tempDist;
					prev[v.row][v.col] = { u.row, u.col };
					Item item(v, tempDist);
					pq.push(item);
				}
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
	Cdt u = finalTarget;
	while (prev[u.row][u.col].row != -1 && prev[u.row][u.col].col != -1) {
		S.push_back(u);
		u = { prev[u.row][u.col].row, prev[u.row][u.col].col };
	}
	S.push_back(src);

	// Print Path
	while (!S.empty()) {
		Cdt temp = S.back();
		S.pop_back();
		printf("%d %d // %f, %f\n", temp.row, temp.col, corners[temp.row][temp.col].latitude, corners[temp.row][temp.col].longitude);
		if (temp.row == finalTarget.row && temp.col == finalTarget.col) {
			printf("parking : [%f, %f]\n", corners[temp.row][temp.col].destination->latitude, corners[temp.row][temp.col].destination->longitude);
		}
	}

	return nextSource;
}

double getBearing(Vertex point1, Vertex point2) // 방위각을 구한다! (point1에서 point2를 향하는 각도)
{
	double P1_latitude = point1.latitude;
	double P1_longitude = point1.longitude;
	double P2_latitude = point2.latitude;
	double P2_longitude = point2.longitude;

	// 현재 위치 : 위도나 경도는 지구 중심을 기반으로 하는 각도이기 때문에 라디안 각도로 변환한다.
	double Cur_Lat_radian = P1_latitude * (3.141592 / 180);
	double Cur_Lon_radian = P1_longitude * (3.141592 / 180);


	// 목표 위치 : 위도나 경도는 지구 중심을 기반으로 하는 각도이기 때문에 라디안 각도로 변환한다.
	double Dest_Lat_radian = P2_latitude * (3.141592 / 180);
	double Dest_Lon_radian = P2_longitude * (3.141592 / 180);

	// radian distance
	double radian_distance = 0;
	radian_distance = acos(sin(Cur_Lat_radian) * sin(Dest_Lat_radian) + cos(Cur_Lat_radian) * cos(Dest_Lat_radian) * cos(Cur_Lon_radian - Dest_Lon_radian));

	// 목적지 이동 방향을 구한다.(현재 좌표에서 다음 좌표로 이동하기 위해서는 방향을 설정해야 한다. 라디안값이다.
	double radian_bearing = acos((sin(Dest_Lat_radian) - sin(Cur_Lat_radian) * cos(radian_distance)) / (cos(Cur_Lat_radian) * sin(radian_distance)));        // acos의 인수로 주어지는 x는 360분법의 각도가 아닌 radian(호도)값이다.

	double true_bearing = 0;
	if (sin(Dest_Lon_radian - Cur_Lon_radian) < 0)
	{
		true_bearing = radian_bearing * (180 / 3.141592);
		true_bearing = 360 - true_bearing;
	}
	else
	{
		true_bearing = radian_bearing * (180 / 3.141592);
	}

	return true_bearing;
}

double getDistance(Vertex point1, Vertex point2)
{
	// 두 점(위도, 경도) 사이의 거리를 구한다
	double lat1 = point1.latitude;
	double lon1 = point1.longitude;
	double lat2 = point2.latitude;
	double lon2 = point2.longitude;

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