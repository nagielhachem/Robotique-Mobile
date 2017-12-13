#pragma once

#include <algorithm>
#include <cfloat>
#include <climits>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <utility>



using namespace std;

struct edge
{
    int to;
    float length;
};

typedef std::vector<int> path;
typedef std::vector<edge> node;
typedef std::vector<node> graph;

struct result
{
    float distance;
    path p;
};

struct content
{
    int pos_x;
    int pos_y;
    int obj_x;
    int obj_y;
    int height;
    int width;
    vector< vector<int> > map;
};



#include "dijkstra.cpp"
