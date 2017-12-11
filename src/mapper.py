from __future__ import division
import numpy as np
import sys


def create_maze(file_path):
    map_data = get_map(file_path)

    x_min, y_min, x_max, y_max, width, height = get_borders(map_data)
    obstacles = get_obstacles(map_data, x_min, y_min)

    start_pos, final_pos = map_data['start_pos'], map_data['final_pos'] 
    start_pos = (start_pos[0] - x_min, start_pos[1] - y_min)
    final_pos = (final_pos[0] - x_min, final_pos[1] - y_min)

    maze  = np.zeros((height, width), dtype=int)
    for x, y in obstacles:
        maze[x, y] = 1
    return maze, start_pos, final_pos


def get_obstacles(map_data, x_min, y_min):
    obstacles = []
    for line in map_data['lines']:
        x0, y0, x1, y1 = line
        pts = get_line((x0, y0), (x1, y1))
        obstacles.extend(pts)

    obstacles = np.array(obstacles)
    obstacles[:, 0] -= x_min
    obstacles[:, 1] -= y_min

    aux = []
    for y, x in obstacles:
        aux.append((x,y))
    obstacles = np.array(aux)

    return obstacles


def get_borders(map_data):
    X = [map_data['start_pos'][0], map_data['final_pos'][0]]
    Y = [map_data['start_pos'][1], map_data['final_pos'][1]]
    for line_coords in map_data['lines']:
        X.extend([line_coords[0], line_coords[2]])
    Y.extend([line_coords[1], line_coords[3]])

    x_min, x_max = min(X), max(X)
    y_min, y_max = min(Y), max(Y)

    width = x_max - x_min + 10
    height = y_max - y_min + 10
    return x_min, y_min, x_max, y_max, width, height


def get_map(map_path):
    with open(map_path, 'r') as f:
        file_lines = f.readlines()

    map_data = dict()
    line_params = False
    lines = []

    for line in file_lines:
        words = line.split()
        if line_params and nb_lines > 0:
            lines.append((int(words[0]), int(words[1]), int(words[2]), int(words[3])))
            nb_lines -= 1
        elif words[0] == 'Cairn:':
            if words[1] == 'RobotHome':
                map_data['start_pos'] = (int(words[2]), int(words[3]))
            elif words[1] == 'Goal':
                map_data['final_pos'] = (int(words[2]), int(words[3]))
        elif words[0] == 'LINES':
            line_params = True
        elif words[0] == 'NumLines:':
            map_data['nb_lines'] = int(words[1])
            nb_lines = int(words[1])

    map_data['lines'] = lines
    return map_data


def get_line(start, end):
    x1, y1 = start
    x2, y2 = end
    dx = x2 - x1
    dy = y2 - y1

    is_steep = abs(dy) > abs(dx)

    if is_steep:
        x1, y1 = y1, x1
        x2, y2 = y2, x2

    swapped = False
    if x1 > x2:
        x1, x2 = x2, x1
        y1, y2 = y2, y1
        swapped = True

    dx = x2 - x1
    dy = y2 - y1
    error = int(dx / 2.0)
    ystep = 1 if y1 < y2 else -1

    y = y1
    points = []
    for x in range(x1, x2 + 1):
        coord  = (y, x) if is_steep else (x, y)
        error -= abs(dy)
        points.append(coord)

        if error < 0:
            y += ystep
            error += dx

    if swapped:
        points.reverse()

    return points


def main(argv):
    if len(argv) == 1:
        print("Usage: Python3 {} map_file".format(argv[0]))
        return -1
    maze, pos, obj = create_maze(sys.argv[1])

    f = open(sys.argv[1][:-4] + ".csv", 'w')

    buf  = "%d %d\n" % (len(maze), len(maze[0]))
    buf += "%d %d\n" % pos
    buf += "%d %d\n" % obj
    for line in maze:
        for val in line:
            buf += str(val) + " "
        buf += "\n"
    f.write(buf)
    f.close()

if __name__ == "__main__":
    main(sys.argv[:]) 
