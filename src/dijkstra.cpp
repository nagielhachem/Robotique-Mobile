#include <utility>
#include <iostream>
#include <vector>
#include <set>
#include <climits>
#include <cfloat>
#include <algorithm>

struct edge
{
	int to;
	float length;
};

using node = std::vector<edge>;
using graph = std::vector<node>;

void add_edge(graph& g, int start, int finish, float length)
{
	if ((int)g.size() <= (std::max)(start, finish))
		g.resize((std::max)(start,finish) + 1);
	g[start].push_back( {finish, length} );
	g[finish].push_back( {start, length} );
}

using path = std::vector<int>;

struct result
{
	float distance;
	path p;
};

result dijkstra(const graph &graph, int source, int target)
{
	std::vector<float> min_distance(graph.size(), FLT_MAX);
	min_distance[source] = 0;
	std::set< std::pair<int,int> > active_vertices;
	active_vertices.insert({0, source});

	while (!active_vertices.empty())
	{
		int where = active_vertices.begin()->second;
		if (where == target)
		{
			float cost = min_distance[where];
			// std::cout << "cost is " << cost << std::endl;
			path p{where};
			while (where != source) 
			{
				int next = where;
				for (edge e : graph[where])
				{
					// std::cout << "examine edge from " << where << "->" << e.to << " length " << e.length << ":";

					if (min_distance[e.to] == FLT_MAX)
					{
						// std::cout << e.to << " unexplored" << std::endl;
						continue;
					}

					if (e.length + min_distance[e.to] != min_distance[where])
					{
						// std::cout << e.to << " not on path" << std::endl;
						continue;
					}
					next = e.to;
					p.push_back(next);
					// std::cout << "backtracked to " << next << std::endl;
					break;
				}
				if (where == next)
				{
					// std::cout << "got lost at " << where << std::endl;
					break;
				}
				where = next;
			}
			std::reverse(p.begin(), p.end());
			return {cost, std::move(p)};
		}
		active_vertices.erase( active_vertices.begin() );
		for (auto ed : graph[where]) 
		{
			if (min_distance[ed.to] > min_distance[where] + ed.length)
			{
				active_vertices.erase({min_distance[ed.to], ed.to});
				min_distance[ed.to] = min_distance[where] + ed.length;
				active_vertices.insert({min_distance[ed.to], ed.to});
			}
		}
	}
	return {FLT_MAX};
}

