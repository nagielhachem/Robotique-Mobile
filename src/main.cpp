#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;



bool is_visible(int x1, int y1, int x2, int y2, vector< vector<int> > map)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    bool is_steep = abs(dy) - abs(dx);
    if (is_steep)
    {
        x1, y1 = y1, x1;
        x2, y2 = y2, x2;
    }

    bool swapped = false;
    if (x1 > x2)
    {
        x1, x2  = x2, x1;
        y1, y2  = y2, y1;
        swapped = true;
    }

    dx          = x2 - x1;
    dy          = y2 - y1;
    int ystep   = (y1 < y2) ? 1: -1;
    float error = (int) (dx / 2.0);

    int y = y1;
    for (int x = x1; x < x2 + 1; x++)
    {
        int c_x = (is_steep) ? y: x;
        int c_y = (is_steep) ? x: y;
        error  -= abs(dy);

        if (map[(is_steep) ? c_y: c_x][(is_steep) ? c_x: c_y] != 0)
            return false;

        if (error < 0)
        {
            y  += ystep;
            error += dx;
        }
    }

    return true;
}



int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    // Get file
    ifstream file;
    file.open(argv[1]);

    // Get metadata
    int width, height, pos_x, pos_y, obj_x, obj_y;
    file >> height >> width;
    file >> pos_y >> pos_x;
    file >> obj_y >> obj_x;

    // Get map
    vector< vector<int> > map;
    map.resize(width, vector<int>(height, 0));
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
            file >> map[i][j];
    file.close();

    // Get nodes
    int p = 1;
    vector< pair<int, int> > nodes = vector< pair<int, int> >();
    nodes.push_back(pair<int, int>(pos_x, pos_y));
    nodes.push_back(pair<int, int>(obj_x, obj_y));
    for (int i = p; i < width - p; i++)
    {
        for (int j = p; j < height - p; j++)
        {
            int sum = map[i - p][j - p] + map[i - p][j]
                    + map[i - p][j + p] + map[i][j - p]
                    + map[i + p][j - p] + map[i][j + p]
                    + map[i + p][j + p] + map[i + p][j];
            if (sum == 1 && map[i][j] != 1)
                nodes.push_back(pair<int, int>(i, j));
        }
    }

    // Get adjacences
    vector< vector<float> > adjas = vector< vector<float> >();
    adjas.resize(nodes.size(), vector<float>(nodes.size(), 0));
    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = i + 1; j < nodes.size(); j++)
        {
            float adja = is_visible(nodes[i].first, nodes[i].second,
                                    nodes[j].first, nodes[j].second,
                                    map)
                        * sqrt(pow(nodes[i].first  - nodes[j].first,  2)
                             + pow(nodes[i].second - nodes[j].second, 2));
            std::cout << nodes[i].first << " " << nodes[i].second << std::endl; // DEBUG
            std::cout << nodes[j].first << " " << nodes[j].second << std::endl; // DEBUG
            std::cout << adja << std::endl; // DEBUG
            adjas[i][j] = adja;
        }
    }

    return 0;
}
