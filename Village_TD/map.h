#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"
#include <SDL.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "route.h"


//remove leading and trailing whitespaces and tabs
std::string trim_str(const std::string& str) {
	size_t start = str.find_first_not_of(" \t");
	if (start == std::string::npos) return "";
	size_t end = str.find_last_not_of(" \t");
	return str.substr(start, end - start + 1);
}


class Map
{

public:
	typedef std::unordered_map<int, Route> SpawnRoutePool;

public:
	Map() = default;
	~Map() = default;

	bool load(const std::string& path) {
		std::ifstream file(path);
		if (!file.is_open()) return false;

		TileMap temp_map;

		int idx_x = -1;
		int idx_y = -1;

		std::string str_line;
		while (std::getline(file, str_line))
		{
			str_line = trim_str(str_line);
			if (str_line.empty()) continue;

			idx_x = -1;
			idx_y++;

			temp_map.emplace_back();

			std::string str_tile;
			std::stringstream ss(str_line);
			while (std::getline(ss, str_tile, ','))
			{
				idx_x++;
				temp_map[idx_y].emplace_back();
				Tile& tile = temp_map[idx_y].back();
				load_tile_from_string(tile, str_tile);
			}
		}
		file.close();

		if (temp_map.empty() || temp_map[0].empty()) return false;

		tile_map = temp_map;
		return true;
	}

	size_t get_width() const { 
		if (tile_map.empty()) return 0;
		return tile_map[0].size(); 
	}

	size_t get_height() const { 
		return tile_map.size(); 
	}

	const TileMap& get_tile_map() const { 
		return tile_map; 
	}

	const SDL_Point& get_home_idx() const { 
		return home_idx; 
	}

	const SpawnRoutePool& get_spawn_route_pool() const { 
		return spawn_route_pool; 
	}

	const Route& get_spawn_route(int route_id) const {
		auto it = spawn_route_pool.find(route_id);
		if (it == spawn_route_pool.end()) return Route();
		return it->second;
	}

	void place_tower(const SDL_Point& idx) {
		if (idx.x < 0 || idx.x >= get_width() || idx.y < 0 || idx.y >= get_height()) return;
		tile_map[idx.y][idx.x].has_tower = true;
	}

private:
	TileMap tile_map;
	SDL_Point home_idx = { 0, 0 };
	SpawnRoutePool spawn_route_pool;


	void load_tile_from_string(Tile& tile, const std::string& str_tile) {
		std::string str_tidy = trim_str(str_tile);
		std::string str_value;
		std::vector<int> values;
		std::stringstream ss(str_tidy);

		while (std::getline(ss, str_value, '\\')) {
			int value;
			try {
				value = std::stoi(str_value);
			}
			catch (std::invalid_argument& e) {
				value = -1;
			}
			values.push_back(value);
		}

		tile.terrian= (values.size() < 1 || values[0] < 0) ? 0 : values[0];
		tile.decoration = (values.size() < 2) ? -1 : values[1];
		tile.direction = (Tile::Direction)((values.size() < 3 || values[2] < 0) ? Tile::Direction::None : (Tile::Direction)values[2]);
		tile.special_flag = (values.size() < 4) ? -1 : values[3];
	}
	void generate_map_cache() {
		for (int y = 0; y < get_width(); y++) {
			for (int x = 0; x < get_height(); x++) {
				Tile& tile = tile_map[y][x];

				//nothing special at this tile
				if (tile.special_flag < 0) continue;

				//home idx tile
				if (tile.special_flag == 0) {
					home_idx = { x, y };
				}

				//spawn tile, populate spawn route pool
				else {
					spawn_route_pool[tile.special_flag] = Route(tile_map, { x, y };
				}
			}
		}
	}
};


#endif // !_MAP_H_