#include "main.hh"
#include "Aria.h"

#define STEP_TIME 10
#define STOP_TIME 2



bool is_visible(int x1, int y1, int x2, int y2, vector< vector<int> > map)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    bool is_steep = abs(dy) > abs(dx);
    if (is_steep)
    {
        swap(x1, y1);
        swap(x2, y2);
    }

    bool swapped = false;
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
        swapped = true;
    }

    dx        = x2 - x1;
    dy        = y2 - y1;
    int ystep = (y1 < y2) ? 1: -1;
    int error = (int) (dx / 2.0);

    int y = y1;
    for (int x = x1; x < x2 + 1; x++)
    {
        int c_x = (is_steep) ? y: x;
        int c_y = (is_steep) ? x: y;
        error  -= abs(dy);

        if (map[(is_steep) ? y: x][(is_steep) ? x: y] != 0)
            return false;

        if (error < 0)
        {
            y  += ystep;
            error += dx;
        }
    }

    return true;
}


void set_values(ArRobot& robot, float vel, float rot)
{
    robot.lock();
    robot.setVel(vel    / STEP_TIME);
    robot.setRotVel(rot / STEP_TIME);

    std::cout << vel / STEP_TIME << std::endl; // DEBUG
    std::cout << rot / STEP_TIME << std::endl; // DEBUG

    robot.unlock();
    ArUtil::sleep(1000 * STEP_TIME);

    robot.lock();
    robot.stop();
    robot.unlock();
    ArUtil::sleep(1000 * STOP_TIME);
}


void go_to_node(ArRobot& robot, pair<int, int> pos, pair<int, int> obj)
{
    robot.lock();
    pair<float, float> direction = pair<int, int>(obj.first  - pos.first,
                                                  obj.second - pos.second);
    float angle = atan2(direction.first, direction.second) * 180 / M_PI - robot.getTh();
    float distance = sqrt(pow(pos.first  - obj.first,  2)
                        + pow(pos.second - obj.second, 2));
    robot.unlock();

    std::cout << "--------------------" << std::endl; // DEBUG
    std::cout << pos.first << " " << pos.second << std::endl; // DEBUG
    std::cout << obj.first << " " << obj.second << std::endl; // DEBUG
    std::cout << angle << " " << distance << std::endl; // DEBUG

    set_values(robot, 0, angle);
    set_values(robot, distance, 0);
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
    file >> pos_y  >> pos_x;
    file >> obj_y  >> obj_x;

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
            int sum = map[i - 1][j - 1] + map[i - 1][j]
                    + map[i - 1][j + 1] + map[i][j - 1]
                    + map[i + 1][j - 1] + map[i][j + 1]
                    + map[i + 1][j + 1] + map[i + 1][j];
            if (sum == 1 && map[i][j] == 1)
            {
                nodes.push_back(pair<int, int>(i + p, j + p));
                nodes.push_back(pair<int, int>(i + p, j - p));
                nodes.push_back(pair<int, int>(i - p, j + p));
                nodes.push_back(pair<int, int>(i - p, j - p));
            }
        }
    }

    // Get the graph
    graph g;
    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = i + 1; j < nodes.size(); j++)
        {
            float adja = is_visible(nodes[i].first, nodes[i].second,
                                    nodes[j].first, nodes[j].second,
                                    map)
                        * sqrt(pow(nodes[i].first  - nodes[j].first,  2)
                             + pow(nodes[i].second - nodes[j].second, 2));
            if (adja != 0)
            {
                add_edge(g, i, j, adja);
                std::cout << i << " " << j << std::endl; // DEBUG
            }
        }
    }
    result res = dijkstra(g, 0, 1);

    std::cout << "Total distance:\t" << res.distance << std::endl;
    std::cout << "Path:" << std::endl;
    for (auto i: res.p)
        std::cout << nodes[i].first << "\t" << nodes[i].second << std::endl;

    // Init aria robot
    Aria::init();
    ArRobot robot;
    argc = 1;
    ArArgumentParser parser(&argc, argv);
    parser.loadDefaultArguments();

    // Connect robot
    ArRobotConnector robotConnector(&parser, &robot);
    if (! robotConnector.connectRobot())
    {
        ArLog::log(ArLog::Terse, "simpleMotionCommands: Could not connect to the robot.");
        if (parser.checkHelpAndWarnUnparsed())
        {
            Aria::logOptions();
            Aria::exit(1);
            return 1;
        }
    }
    if (! Aria::parseArgs())
    {
        Aria::logOptions();
        Aria::exit(1);
        return 1;
    }
    robot.runAsync(true);
    robot.lock();
    robot.enableMotors();
    robot.unlock();
    ArUtil::sleep(1000);

    // Go to place
    for (int i = 1; i < res.p.size(); i++)
        go_to_node(robot,
                   nodes[res.p[i - 1]],
                   nodes[res.p[i]]);

    // Exit
    std::cout << "Done" << std::endl;
    robot.stopRunning();
    robot.waitForRunExit();
    Aria::exit(0);

    return 0;
}
