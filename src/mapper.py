from __future__ import division
import numpy as np
import sys



def create_search_maze(map_file_path):
    map_data = get_map(map_file_path)

    x_min, y_min, x_max, y_max, width, height = get_extrema(map_data)
    obstacle_coordinates = generate_obstacle_coordinates(map_data, x_min, y_min)

    start_pos, final_pos = map_data['start_pos'], map_data['final_pos'] 
    start_pos = (start_pos[0] - x_min, start_pos[1] - y_min)
    final_pos = (final_pos[0] - x_min, final_pos[1] - y_min)

    maze  = np.zeros((height, width), dtype=int)
    for x, y in obstacle_coordinates:
        maze[x, y] = 1
    return maze, start_pos, final_pos


def get_path(maze, start_pos, final_pos):
    path = np.array(astar(maze, start_pos, final_pos))
    filtered_path = np.array(filter_path(path))
    return filtered_path


def generate_obstacle_coordinates(map_data, x_min, y_min):
    obstacle_coordinates = []
    for line in map_data['lines']:
        x0, y0, x1, y1 = line
        points = get_line((x0, y0), (x1, y1))
        obstacle_coordinates.extend(points)

    obstacle_coordinates       = np.array(obstacle_coordinates)
    obstacle_coordinates[:,0] -= x_min
    obstacle_coordinates[:,1] -= y_min

    aux = []
    for y, x in obstacle_coordinates:
        aux.append((x,y))
    obstacle_coordinates = np.array(aux)

    return obstacle_coordinates


def get_extrema(map_data):
    x_pts = [map_data['start_pos'][0], map_data['final_pos'][0]]
    y_pts = [map_data['start_pos'][1], map_data['final_pos'][1]]
    for line_coords in map_data['lines']:
        x_pts.extend([line_coords[0], line_coords[2]])
    y_pts.extend([line_coords[1], line_coords[3]])

    x_min, x_max = min(x_pts), max(x_pts)
    y_min, y_max = min(y_pts), max(y_pts)

    width = x_max - x_min + 10
    height = y_max - y_min + 10
    return x_min, y_min, x_max, y_max, width, height


def get_map(map_path):
    with open(map_path, 'r') as f:
        content = f.readlines()

    map_data = dict()
    next_is_line_infos = False
    lines = []

    for line in content:
        aux = line.split()
        if next_is_line_infos and nb_lines > 0:
            lines.append((int(aux[0]), int(aux[1]), int(aux[2]), int(aux[3])))
            nb_lines -= 1
        elif aux[0] == 'Cairn:':
            if aux[1] == 'RobotHome':
                map_data['start_pos'] = (int(aux[2]), int(aux[3]))
            elif aux[1] == 'Goal':
                map_data['final_pos'] = (int(aux[2]), int(aux[3]))
        elif aux[0] == 'LINES':
            next_is_line_infos = True
        elif aux[0] == 'NumLines:':
            map_data['nb_lines'] = int(aux[1])
            nb_lines = int(aux[1])

    map_data['lines'] = lines
    return map_data


def filter_path(path):
    if len(path) == 0:
        return None
    else:
        pts = []
        i = 0
        if path[0][0] == path[1][0]:
            horizontal = False
        else:
            horizontal = True

        while i < path.shape[0] - 1:
            if horizontal:
                while i < path.shape[0] - 1 and path[i+1][0] == path[i][0]:
                    i += 1
                horizontal = False
            else:
                while i < path.shape[0] - 1 and path[i+1][1] == path[i][1]:
                    i += 1
                horizontal = True
            pts.append(path[i])
        return pts



def get_line(start, end):
    x1, y1 = start
    x2, y2 = end
    dx     = x2 - x1
    dy     = y2 - y1

    is_steep = abs(dy) > abs(dx)

    if is_steep:
        x1, y1 = y1, x1
        x2, y2 = y2, x2

    swapped = False
    if x1 > x2:
        x1, x2 = x2, x1
        y1, y2 = y2, y1
        swapped = True

    dx    = x2 - x1
    dy    = y2 - y1
    error = int(dx / 2.0)
    ystep = 1 if y1 < y2 else -1

    y      = y1
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
    maze, pos, obj = create_search_maze(sys.argv[1])

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
