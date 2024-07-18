#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

#include "manager.h"
#include <string>
#include <iostream>
#include <cJSON.h>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include "map.h"
#include "wave.h"


class ConfigManager : public Manager<ConfigManager>
{
	friend class Manager<ConfigManager>;

protected:
	ConfigManager() = default;
	~ConfigManager() = default;

private:
	struct BasicTemplate {
		std::string window_title = "Village_TD";	
		int window_width = 1280;
		int window_height = 720;
	};

	struct playerTemplate {
		double speed = 3;
		double normal_attack_interval = 0.5;
		double nomral_attack_damage = 0;
		double skill_interval = 5;
		double skill_damage = 1;
	};

	struct towerTemplate {
		double interval[10] = { 1 };
		double damage[10] = { 25 };
		double range[10] = { 5 };
		double build_cost [10] = { 50 };
		double upgrade_cost[9] = { 50 };
	};

	struct enemyTemplate {
		double speed = 1;
		double hp = 100;
		double damage = 1;
		double reward_ratio = 0.5;
		double recover_interval = 10;
		double recover_range = 0;
		double recover_amount = 25;

	};

public:
	Map map;
	std::vector<Wave> waves;
	
	int level_archor = 0;
	int level_axeman = 0;
	int level_gunner = 0;

	bool is_game_win = true;
	bool is_game_over = false;

	//generate surface from tile_map
	SDL_Rect rect_tile_map = { 0 };

	BasicTemplate basic_template;
	
	playerTemplate player_template;
	
	towerTemplate archer_template;
	towerTemplate axeman_template;
	towerTemplate gunner_template;
	
	enemyTemplate slim_template;
	enemyTemplate slim_king_template;
	enemyTemplate skeleton_template;
	enemyTemplate goblin_template;
	enemyTemplate goblin_preest_template;

		
	const double initial_hp = 10;
	const double initial_coin = 100;
	const double coin_drop_amount = 20;

public:
	bool load_level_config(const std::string& path) {
		std::ifstream file(path);
		if (!file.good()) return false;

		std::stringstream str_stream;
		str_stream << file.rdbuf();
		file.close();

		cJSON* json_root = cJSON_Parse(str_stream.str().c_str());

		if (!json_root) {
			std::cerr << "Error: " << cJSON_GetErrorPtr() << std::endl;
			return false;
		}
		if (json_root->type != cJSON_Array) {
			cJSON_Delete(json_root);
			std::cerr << "Error: root is not an array" << std::endl;
			return false;
		}

		cJSON* json_wave = nullptr;
		cJSON_ArrayForEach(json_wave, json_root) {
			
			if (json_wave->type != cJSON_Object) {
				continue;
			}

			waves.emplace_back();
			Wave& wave = waves.back();

			cJSON* json_wave_rewards = cJSON_GetObjectItem(json_wave, "rewards");
			if (json_wave_rewards && json_wave_rewards->type == cJSON_Number) {
				wave.rewards = json_wave_rewards->valuedouble;
			}

			cJSON* json_wave_interval = cJSON_GetObjectItem(json_wave, "interval");
			if (json_wave_interval && json_wave_interval->type == cJSON_Number) {
				wave.interval = json_wave_interval->valuedouble;
			}

			cJSON* json_wave_spawn_list = cJSON_GetObjectItem(json_wave, "enemies");
			if (json_wave_spawn_list && json_wave_spawn_list->type == cJSON_Array) {
				cJSON* json_spawn_event = nullptr;
				cJSON_ArrayForEach(json_spawn_event, json_wave_spawn_list) {
					
					if (json_spawn_event->type != cJSON_Object) {
						continue;
					}

					wave.spawn_events.emplace_back();
					Wave::SpawnEvent& spawn_event = wave.spawn_events.back();

					cJSON* json_spawn_event_interval = cJSON_GetObjectItem(json_spawn_event, "interval");
					if (json_spawn_event_interval && json_spawn_event_interval->type == cJSON_Number) {
						spawn_event.interval = json_spawn_event_interval->valuedouble;
					}

					cJSON* json_spawn_event_spawn_point = cJSON_GetObjectItem(json_spawn_event, "point");
					if (json_spawn_event_spawn_point && json_spawn_event_spawn_point->type == cJSON_Number) {
						spawn_event.spawn_point = json_spawn_event_spawn_point->valueint;
					}

					cJSON* json_spawn_event_enemy_type = cJSON_GetObjectItem(json_spawn_event, "enemy");
					if (json_spawn_event_enemy_type->type == cJSON_String) {
						const std::string enemy_type = json_spawn_event_enemy_type->valuestring;
						if (enemy_type == "Slim") {
							spawn_event.enemy_type = EnemyType::Slim;
						}
						else if (enemy_type == "SlimKing") {
							spawn_event.enemy_type = EnemyType::SlimKing;
						}
						else if (enemy_type == "Skeleton") {
							spawn_event.enemy_type = EnemyType::Skeleton;
						}
						else if (enemy_type == "Goblin") {
							spawn_event.enemy_type = EnemyType::Goblin;
						}
						else if (enemy_type == "GoblinPriest") {
							spawn_event.enemy_type = EnemyType::GoblinPriest;
						}
					}

				}
				if (wave.spawn_events.empty())
					waves.pop_back();
			}
		}
		cJSON_Delete(json_root);

		if (waves.empty()) {
			std::cerr << "Error: no wave loaded" << std::endl;
			return false;
		}
		return true;
	}

	bool load_game_config(const std::string& path) {
	
	}
};	 



#endif // !_CONFIG_MANAGER_H_