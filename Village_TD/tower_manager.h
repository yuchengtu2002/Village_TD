#pragma once

#include "tower.h"
#include "resource_manager.h"
#include "tower_type.h"
#include "manager.h"
#include "config_manager.h"
#include  "archer_tower.h"
#include "gunner_tower.h"
#include "axeman_tower.h"

#include <vector>

class TowerManager : public Manager<TowerManager>
{
	friend class Manager<TowerManager>;

public:
	void on_update(double delta)
	{
		for (auto& tower : tower_list)
		{
			tower->on_update(delta);
		}
	}

	void on_render(SDL_Renderer* renderer)
	{
		for (auto& tower : tower_list)
		{
			tower->on_render(renderer);
		}
	}

	double get_place_cost(TowerType type)
	{
		static ConfigManager* config = ConfigManager::instance();

		switch (type)
		{
		case TowerType::Archer:
			return config->archer_template.build_cost[config->level_archer];
		case TowerType::Gunner:
			return config->gunner_template.build_cost[config->level_gunner];	
		case TowerType::Axeman:
			return config->axeman_template.build_cost[config->level_axeman];	
		default:
			return 0;
		}
	}

	double get_upgrade_cost(TowerType type)
	{
		static ConfigManager* config = ConfigManager::instance();

		switch (type)
		{
		case TowerType::Archer:
			if (config->level_archer == 9) return -1;
			else return config->archer_template.upgrade_cost[config->level_archer];
		case TowerType::Gunner:
			if (config->level_gunner == 9) return -1;
			else return config->gunner_template.upgrade_cost[config->level_gunner];
		case TowerType::Axeman:
			if (config->level_axeman == 9) return -1;
			else return config->axeman_template.upgrade_cost[config->level_axeman];
		default:
			return 0;
		}
	}

	double get_damage_range(TowerType type) {
		static ConfigManager* config = ConfigManager::instance();

		switch (type)
		{
		case TowerType::Archer:
			return config->archer_template.range[config->level_archer];
		case TowerType::Gunner:
			return config->gunner_template.range[config->level_gunner];
		case TowerType::Axeman:
			return config->axeman_template.range[config->level_axeman];
		default:
			return 0;
		}
	}

	void place_tower(TowerType type, const SDL_Point& idx) {
		Tower* tower = nullptr;

		switch (type) {
		case Archer:
			tower = new ArcherTower();
			break;
		case Gunner:
			tower = new GunnerTower();
			break;
		case Axeman:
			tower = new AxemanTower();
			break;
		default:
			tower = new ArcherTower();
		}
		static Vector2 position;
		static const SDL_Rect& rect = ConfigManager::instance()->rect_tile_map;
		position.x = rect.x + idx.x * TILE_SIZE + TILE_SIZE / 2;
		position.y = rect.y + idx.y * TILE_SIZE + TILE_SIZE / 2;

		tower->set_position(position);
		tower_list.push_back(tower); 
		
		ConfigManager::instance()->map.place_tower(idx);
		
		static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::instance()->get_sound_pool();
		Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_PlaceTower)->second, 0);
	}


	void upgrade_tower(TowerType type) {
		static ConfigManager* config = ConfigManager::instance();

		switch (type)
		{
		case TowerType::Archer:
			if (config->level_archer == 9) return;
			config->level_archer++;
			break;
		case TowerType::Gunner:
			if (config->level_gunner == 9) return;
			config->level_gunner++;
			break;
		case TowerType::Axeman:
			if (config->level_axeman == 9) return;
			config->level_axeman++;
			break;
		}
		static const ResourcesManager::SoundPool& sound_pool
			= ResourcesManager::instance()->get_sound_pool();

		Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_TowerLevelUp)->second, 0);
	}

	void reset() {
		for (auto& tower : tower_list) {
			delete tower;
		}
		tower_list.clear();
	}

protected:
	TowerManager() = default;
	~TowerManager() = default;

private:
	std::vector<Tower*> tower_list;

};