#pragma once

#include "manager.h"
#include "enemy.h"
#include "SDL.h"
#include "config_manager.h"
#include "home_manager.h"
#include <vector>

class EnemyManager : public Manager<EnemyManager>
{
	friend class Manager<EnemyManager>;

public:
	typedef std::vector<Enemy*> EnemyList;

	void on_update(double delta) {
		for (auto& enemy : enemy_list) {
			enemy->on_update(delta);
		}
		process_bullet_collision();
		process_home_collision();
	}

	void on_render(SDL_Renderer* renderer) {
		for (auto& enemy : enemy_list) {
			enemy->on_render(renderer);
		}
	}



protected:
	EnemyManager() = default;
	~EnemyManager() {
		for (auto& enemy : enemy_list) {
			delete enemy;
		}
	};


private:
	EnemyList enemy_list;

private:
	void process_home_collision() {
		static ConfigManager* config = ConfigManager::instance();
		static const SDL_Point& home_idx = config->map.get_home_idx();
		static const SDL_Rect& rect_tile_map = config->rect_tile_map;

		static const Vector2 position_home_tile = 
		{
			(double)rect_tile_map.x + home_idx.x * TILE_SIZE,
			(double)rect_tile_map.y + home_idx.y * TILE_SIZE
		};

		for (Enemy* enemy : enemy_list) {
			if (enemy->can_remove()) continue;

			const Vector2& position = enemy->get_position();

			//if enemy is in home tile
			if (position.x >= position_home_tile.x &&
				position.x <= position_home_tile.x + TILE_SIZE &&
				position.y >= position_home_tile.y &&
				position.y <= position_home_tile.y + TILE_SIZE)
			{
				enemy->make_invalid();
				HomeManager::instance()->decrease_hp(enemy->get_damage());
			}
		}
	}

	void process_bullet_collision() {}

	void remove_invalid_enemies() {
		enemy_list.erase(std::remove_if(enemy_list.begin(), enemy_list.end(),
			[](Enemy* enemy)
			{
				bool removable = enemy->can_remove();
				if (removable) delete enemy;
				return removable;
			}), enemy_list.end());
	}
};
  