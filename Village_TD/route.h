#ifndef _ROUTE_H_
#define _ROUTE_H_

#include "tile.h"
#include <SDL.h>
#include <vector>

class Route {

public:
	typedef std::vector<SDL_Point> IdxList;

public:
	Route() = default;

	Route(const TileMap& map, const SDL_Point& idx_origin) {
		size_t width = map[0].size();
		size_t height = map.size();
		SDL_Point idx_next = idx_origin;

		while (1) {

			//deal with out of bound and duplicate path
			if (idx_next.x < 0 || idx_next.x >= width || idx_next.y < 0 || idx_next.y >= height) break;
			if (check_duplicate_idx(idx_next)) break;
			else idx_list.push_back(idx_next);

			bool is_next_found = true;
			const Tile& tile = map[idx_next.y][idx_next.x];

			//reaches village
			if (tile.special_flag == 0) break;

			//Move next idx according to direction
			switch (tile.direction) {
			case Tile::Direction::Up:
				idx_next.y--;
				break;
			case Tile::Direction::Down:
				idx_next.y++;
				break;
			case Tile::Direction::Left:
				idx_next.x--;
				break;
			case Tile::Direction::Right:
				idx_next.x++;
				break;
			default:
				is_next_found = false;
				break;
			}

			if (!is_next_found) break;
		}
	}
	~Route() = default;

	const IdxList& get_idx_list() const {
		return idx_list;
	}

private:
	IdxList idx_list;

	bool check_duplicate_idx(const SDL_Point& idx) {
		for (const auto& idx_temp : idx_list) {
			if (idx_temp.x == idx.x && idx_temp.y == idx.y) {
				return true;
			}
		}
		return false;
	}

};


#endif
