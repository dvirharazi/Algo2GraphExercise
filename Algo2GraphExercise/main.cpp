#include<stdlib.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <queue>
#include <math.h>
#include <omp.h>
#include <thread>
#include <fstream>
#include <ctime>

#define V 1000
#define num_of_graph 500
using namespace std;

//Function declaration
vector<vector<int>> build_random_graph(int, double);
vector<int> bfs(vector<vector<int>>, int);
void printVecAdj(vector<vector<int>>);
int connectivity(vector<vector<int>>);
int is_Isolted(vector<vector<int>>);
int diameter(vector<vector<int>>);

/*                                                                            Global variables:
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//Arrays that hold the 10 probability
double arrProb1[10] = { 0.0045, 0.005, 0.0055, 0.006, 0.0065 , 0.00691 , 0.00697 , 0.0075, 0.0085,0.0095 };   //-> V=1000 - Treshold1=0.00690776
double arrProb2[10] = { 0.07, 0.08, 0.085, 0.095, 0.1 , 0.12 , 0.125 , 0.13, 0.135,0.14 };		              //-> V=1000 - Treshold2=0.117536
double arrProb3[10] = { 0.0035, 0.004434, 0.0051, 0.006758, 0.0068 , 0.00695 , 0.00698 , 0.007, 0.0082,0.009 }; //-> V=1000 - Treshold3=0.00690776


//A reset array that will hold the probabilities after the calculation
int countIsolate[10] = { 0 };
int countDiam[10] = { 0 };
int countConnnected[10] = { 0 };

//Calculate the threshold
double threshHold1 = log(V) / (double)V;
double threshHold2 = sqrt((2 * log(V)) / (double)V);
double threshHold3 = log(V) / (double)V;

/*																					 main:
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int main() {
	int i, j;
	int resultDiam = 0;
	ofstream algoFileConnected;
	ofstream algoFileDiameter;
	ofstream algoFileIsolate;
	algoFileConnected.open("algoCon.csv");	//Open 3 CSV Files
	algoFileDiameter.open("algoDiam.csv");
	algoFileIsolate.open("algoIso.csv");


	srand((unsigned)time(0));  //Get a seed for random


	for (i = 0; i < 10; i++)
	{

#pragma omp parallel for      //Create a multply threads 
		for (j = 0; j < num_of_graph; j++)
		{
			vector<vector<int>> adjVec1 = build_random_graph(V, arrProb1[i]);	//Crete a new graph with our vertex and probabilty
			vector<vector<int>> adjVec2 = build_random_graph(V, arrProb2[i]);
			vector<vector<int>> adjVec3 = build_random_graph(V, arrProb3[i]);

			if (connectivity(adjVec1))
				countConnnected[i]++;

			resultDiam = diameter(adjVec2);
			if (resultDiam != -1) {
				countDiam[i] += resultDiam;
			}


			if (is_Isolted(adjVec3))
				countIsolate[i]++;
		}
	}



	algoFileConnected << "Connected" << endl;
	algoFileDiameter << "Diameter" << endl;
	algoFileIsolate << "Isolate" << endl;

	for (i = 0; i < 10; i++) {

		algoFileConnected << "p=" << arrProb1[i] << ",";     //Write to CSV file the row with our array probability
		algoFileDiameter << "p=" << arrProb2[i] << ",";
		algoFileIsolate << "p=" << arrProb1[i] << ",";

	}
	algoFileConnected << endl;
	algoFileDiameter << endl;
	algoFileIsolate << endl;

	for (i = 0; i < 10; i++)
	{
		algoFileConnected << countConnnected[i] / (double)(num_of_graph) << ",";    //Write to CSV file numbers of Connected graph divide by the amount of graph    
		algoFileDiameter << countDiam[i] / (double)(num_of_graph) << ",";           //Write to CSV file numbers of Diam graph <= 2 divide by the amount of graph    
		algoFileIsolate << countIsolate[i] / (double)(num_of_graph) << ",";         //Write to CSV file numbers of isolate graph divide by the amount of graph    
	}
	return 0;
}

/*																				Build random graph function:
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

vector<vector<int>> build_random_graph(int v, double p) {
	/*
	  This function gets a number of vertices and probability and creates a new graph
	@param vertices, probability
	   @return graph that presents by vector of vectors
 */
	int i, j;
	vector<vector<int>> adjvector(v);

	for (i = 0; i < v; i++)
	{
		for (j = i + 1; j < v; j++) {
			if (rand() / (double)RAND_MAX <= p) {
				adjvector[i].push_back(j);
				adjvector[j].push_back(i);
			}
		}
	}
	return adjvector;
}

void printVecAdj(vector<vector<int>> adjvector) {
	int i;
	for (i = 0; i < (signed)adjvector.size(); i++)
	{
		cout << "Vertex #" << i << ": [ ";
		for (auto x : adjvector[i]) {
			cout << x << " ";
		}
		cout << "]" << endl;
	}
}


/*																			 connectivity Function:
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int connectivity(vector<vector<int>> adjvector) {
	/*
	   This function gets a graph and checks if is connected or not
		@param Graph adjSet
		   @return 1 if connected, else 0
	 */
	vector<int> distance;

	if (is_Isolted(adjvector)) {
		return 0;
	}

	distance = bfs(adjvector, 0);
	for (auto vertex : distance)
	{
		if (vertex == -1) return 0;
	}
	return 1;
}


/*																					 Isolated function:
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int is_Isolted(vector<vector<int>> adjvector) {

	/*
	This function gets a graph and checks if there is vertex without any neighbors
	  @param Graph adjSet
		@return 1 if there is, else 0
	*/
	int i;
	for (i = 0; i < (signed)adjvector.size(); i++)
	{
		if (adjvector[i].empty())
		{
			return 1;
		}
	}
	return 0;
}

/*																					 BFS Function:
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

vector<int> bfs(vector<vector<int>> adjvector, int u) {
	/*
	This function gets a graph and vertex and does a BFS from any of the graph vertices
	  @param Graph adjSet, vertex u
	   @return Vector Distances
	 */
	vector<int> distanceVertex(V, -1);
	queue<int> q;

	q.push(u);
	distanceVertex[u] = 0;

	while (!q.empty()) {

		int f = q.front();
		q.pop();

		for (auto i = adjvector[f].begin(); i != adjvector[f].end(); i++)
		{
			if (distanceVertex[*i] == -1) {
				q.push(*i);
				distanceVertex[*i] = distanceVertex[f];
				distanceVertex[*i]++;
			}
		}
	}
	return distanceVertex;
}
/*																					 Diameter function:
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

int diameter(vector<vector<int>> adjvector) {
	/*
		This function gets a graph and calc with BFS Algorithm the max diameter between 2 vertices
	  @param Graph adjSet
		@return Vector maxDiam
	*/
	if (!connectivity(adjvector)) return -1;			//-1 -> The diam is infinty
	int maxDiam = 0;
	vector<int> distance(V);

	for (int i = 0; i < (signed)adjvector.size(); i++)
	{
		distance = bfs(adjvector, i);
		for (auto x : distance) {
			if (x > maxDiam)
				maxDiam = x;
		}
	}
	return maxDiam;
}