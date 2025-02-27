#pragma once

#include "manager.h"
#include "enemy.h"
#include "SDL.h"
#include "config_manager.h"
#include "home_manager.h"
#include <vector>
#include "slim_enemy.h"
#include "king_slim_enemy.h"
#include "skeleton_enemy.h"
#include "goblin_enemy.h"
#include "goblin_priest.h"
#include "bullet_manager.h"
#include  "coin_manager.h"

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

		remove_invalid_enemies();
	}

	void on_render(SDL_Renderer* renderer) {
		for (auto& enemy : enemy_list) {
			enemy->on_render(renderer);
		}
	}

	void spawn_enemy(EnemyType type, int idx_spawn_point) {
		static Vector2 position;
		static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		static const Map::SpawnerRoutePool& spawner_route_pool = ConfigManager::instance()->map.get_idx_spawner_pool();

		const auto& it = spawner_route_pool.find(idx_spawn_point);  
		if (it == spawner_route_pool.end()) return;

		Enemy* enemy = nullptr;
		switch (type) {
			case EnemyType::Slim:
			enemy = new SlimEnemy();
			break;
		case EnemyType::KingSlim:
			enemy = new KingSlimEnemy();
			break;
		case EnemyType::Skeleton:
			enemy = new SkeletonEnemy();
			break;
		case EnemyType::Goblin:
			enemy = new GoblinEnemy();
			break;
		case EnemyType::GoblinPriest:
			enemy = new GoblinPriestEnemy();
			break;
		}
		enemy->set_on_skill_released([this](Enemy* enemy_src) {
			double recover_amount = enemy_src->get_recover_amount();
			double recover_range = enemy_src->get_recover_range();

			if (recover_range < 0) return;
			
			const Vector2& position_src = enemy_src->get_position();
			for (auto& enemy_dst : enemy_list) {
				if (enemy_dst->can_remove()) continue;
				const Vector2& position_dst = enemy_dst->get_position();

				double distance = (position_dst - position_src).length();
				if (distance <= recover_range) {
					enemy_dst->recover_hp(recover_amount);
				}


			}
		});

		const Route::IdxList& idx_list = it->second.get_idx_list();
		position.x = rect_tile_map.x + idx_list[0].x * TILE_SIZE + TILE_SIZE/2;
		position.y = rect_tile_map.y + idx_list[0].y * TILE_SIZE + TILE_SIZE/2;

		enemy->set_position(position);
		enemy->set_route(&it->second);
		enemy_list.push_back(enemy);
	}

	bool check_cleared() {
		return enemy_list.empty();
	}

	EnemyManager::EnemyList& get_enemy_list()
	{
		return enemy_list;
	}


public:
	void reset() {
		for (auto& enemy : enemy_list) {
			delete enemy;
		}
		enemy_list.clear();
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

	void process_bullet_collision() {
	
		static BulletManager* bullet_manager = BulletManager::instance();
		static CoinManager* coin_manager = CoinManager::instance();

		for (Enemy* enemy : enemy_list) {
			if (enemy->can_remove()) continue;

			const Vector2& pos_enemy = enemy->get_position();
			const Vector2& size_enemy = enemy->get_size();

			for (Bullet* bullet : bullet_manager->get_bullet_list()) {
				if (!bullet->can_collide()) continue;

				const Vector2& pos_bullet = bullet->get_position();

				if (pos_bullet.x >= pos_enemy.x - size_enemy.x / 2 &&
					pos_bullet.x <= pos_enemy.x + size_enemy.x / 2 &&
					pos_bullet.y >= pos_enemy.y - size_enemy.y / 2 &&
					pos_bullet.y <= pos_enemy.y + size_enemy.y / 2)
				{
					double damage = bullet->get_damage();
					double damage_range = bullet->get_damage_range();

					if (damage_range < 0) {
						enemy->decrease_hp(damage);
						if (enemy->can_remove()) {
							try_spawn_coin_prop(pos_enemy, enemy->get_reward_ratio());
						}
					}
					else {
						const Vector2& pos_bullet = bullet->get_position();
						for (Enemy* enemy_target : enemy_list) {
							if (enemy_target->can_remove()) continue;

							const Vector2& pos_target = enemy_target->get_position();
							double distance = (pos_target - pos_bullet).length();

							if (distance <= damage_range) {
								enemy_target->decrease_hp(damage);
								if (enemy_target->can_remove()) {
									try_spawn_coin_prop(pos_target, enemy_target->get_reward_ratio());
								}
							}
						}
					}
					bullet->on_collide(enemy);
				}
			}
		}
	}

	void remove_invalid_enemies() {
		enemy_list.erase(std::remove_if(enemy_list.begin(), enemy_list.end(),
			[](Enemy* enemy)
			{
				bool removable = enemy->can_remove();
				if (removable) delete enemy;
				return removable;
			}), enemy_list.end());
	}

	void try_spawn_coin_prop(const Vector2& position, double ratio) {
		static ConfigManager* config = ConfigManager::instance();
		static CoinManager* coin_manager = CoinManager::instance();

		if ((double)rand() / 100 <= ratio) {
			coin_manager->create_coin_prop(position);
		}
	}
};
  