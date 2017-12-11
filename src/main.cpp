#include <fstream>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;



int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    // Get file
    ifstream file;
    file.open(argv[1]);

    // Get metadata
    int width, height, pos_x, pos_y, obj_x, obj_y;
    file >> width >> height;
    file >> pos_x >> pos_y;
    file >> obj_x >> obj_y;

    // Get map
    vector< vector<int> > map;
    map.resize(width, vector<int>(height, 0));
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
            file >> map[i][j];

    file.close();

    return 0;
}
