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

public:
	struct BasicTemplate {
		std::string window_title = "Village_TD";	
		int window_width = 1280;
		int window_height = 720;
	};

	struct playerTemplate {
		double speed = 3;
		double normal_attack_interval = 0.5;
		double normal_attack_damage = 0;
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
	
	int level_archer = 0;
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
	enemyTemplate goblin_preist_template;

		
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

			cJSON* json_wave_spawn_list = cJSON_GetObjectItem(json_wave, "spawn_list");
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
						else if (enemy_type == "KingSlim") {
							spawn_event.enemy_type = EnemyType::KingSlim;
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
		std::ifstream file(path);
		if (!file.good()) return false;

		std::stringstream str_stream;
		str_stream << file.rdbuf();
		file.close();
		
		cJSON* json_root = cJSON_Parse(str_stream.str().c_str());
		if (!json_root || json_root->type != cJSON_Object) {
			std::cerr << "Error: " << cJSON_GetErrorPtr() << std::endl;
			return false;
		}

		cJSON* json_basic = cJSON_GetObjectItem(json_root, "basic");
		cJSON* json_player = cJSON_GetObjectItem(json_root, "player");
		cJSON* json_tower = cJSON_GetObjectItem(json_root, "tower");
		cJSON* json_enemy = cJSON_GetObjectItem(json_root, "enemy");

		if (!json_basic || !json_player || !json_tower || !json_enemy
			|| json_basic->type != cJSON_Object
			|| json_player->type != cJSON_Object
			|| json_tower->type != cJSON_Object
			|| json_enemy->type != cJSON_Object) {

			cJSON_Delete(json_root);
			std::cerr << "Error: invalid json format" << std::endl;
			return false;
		}
		parse_basic_template(basic_template, json_basic);

		parse_player_template(player_template, json_player);

		parse_tower_template(archer_template, cJSON_GetObjectItem(json_tower, "archer"));
		parse_tower_template(axeman_template, cJSON_GetObjectItem(json_tower, "axeman"));
		parse_tower_template(gunner_template, cJSON_GetObjectItem(json_tower, "gunner"));

		parse_enemy_template(slim_template, cJSON_GetObjectItem(json_enemy, "slim"));
		parse_enemy_template(slim_king_template, cJSON_GetObjectItem(json_enemy, "slim_king"));
		parse_enemy_template(skeleton_template, cJSON_GetObjectItem(json_enemy, "skeleton"));
		parse_enemy_template(goblin_template, cJSON_GetObjectItem(json_enemy, "goblin"));
		parse_enemy_template(goblin_preist_template, cJSON_GetObjectItem(json_enemy, "goblin_priest"));

		cJSON_Delete(json_root);
		return true;

	}

private:
	void parse_basic_template(BasicTemplate& tpl, cJSON* json_root) {
		if (!json_root || json_root->type != cJSON_Object) return;

		cJSON* json_window_title = cJSON_GetObjectItem(json_root, "window_title");
		if (json_window_title && json_window_title->type == cJSON_String) {
			tpl.window_title = json_window_title->valuestring;
		}

		cJSON* json_window_width = cJSON_GetObjectItem(json_root, "window_width");
		if (json_window_width && json_window_width->type == cJSON_Number) {
			tpl.window_width = json_window_width->valueint;
		}

		cJSON* json_window_height = cJSON_GetObjectItem(json_root, "window_height");
		if (json_window_height && json_window_height->type == cJSON_Number) {
			tpl.window_height = json_window_height->valueint;
		}
	}

	void parse_player_template(playerTemplate& tpl, cJSON* json_root) {
		if (!json_root || json_root->type != cJSON_Object) return;

		cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
		if (json_speed && json_speed->type == cJSON_Number) {
			tpl.speed = json_speed->valuedouble;
		}

		cJSON* json_normal_attack_interval = cJSON_GetObjectItem(json_root, "normal_attack_interval");
		if (json_normal_attack_interval && json_normal_attack_interval->type == cJSON_Number) {
			tpl.normal_attack_interval = json_normal_attack_interval->valuedouble;
		}

		cJSON* json_normal_attack_damage = cJSON_GetObjectItem(json_root, "normal_attack_damage");
		if (json_normal_attack_damage && json_normal_attack_damage->type == cJSON_Number) {
			tpl.normal_attack_damage = json_normal_attack_damage->valuedouble;
		}

		cJSON* json_skill_interval = cJSON_GetObjectItem(json_root, "skill_interval");
		if (json_skill_interval && json_skill_interval->type == cJSON_Number) {
			tpl.skill_interval = json_skill_interval->valuedouble;
		}

		cJSON* json_skill_damage = cJSON_GetObjectItem(json_root, "skill_damage");
		if (json_skill_damage && json_skill_damage->type == cJSON_Number) {
			tpl.skill_damage = json_skill_damage->valuedouble;
		}
	}

	void parse_tower_template(towerTemplate& tpl, cJSON* json_root) {
		if (!json_root || json_root->type != cJSON_Object) return;

		cJSON* json_interval = cJSON_GetObjectItem(json_root, "interval");
		if (json_interval && json_interval->type == cJSON_Array) {
			cJSON* json_interval_item = nullptr;
			int i = 0;
			cJSON_ArrayForEach(json_interval_item, json_interval) {
				if (json_interval_item->type == cJSON_Number) {
					tpl.interval[i] = json_interval_item->valuedouble;
					i++;
				}
			}
		}

		cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
		if (json_damage && json_damage->type == cJSON_Array) {
			cJSON* json_damage_item = nullptr;
			int i = 0;
			cJSON_ArrayForEach(json_damage_item, json_damage) {
				if (json_damage_item->type == cJSON_Number) {
					tpl.damage[i] = json_damage_item->valuedouble;
					i++;
				}
			}
		}

		cJSON* json_range = cJSON_GetObjectItem(json_root, "view_range");
		if (json_range && json_range->type == cJSON_Array) {
			cJSON* json_range_item = nullptr;
			int i = 0;
			cJSON_ArrayForEach(json_range_item, json_range) {
				if (json_range_item->type == cJSON_Number) {
					tpl.range[i] = json_range_item->valuedouble;
					i++;
				}
			}
		}

		cJSON* json_build_cost = cJSON_GetObjectItem(json_root, "build_cost");
		if (json_build_cost && json_build_cost->type == cJSON_Array) {
			cJSON* json_build_cost_item = nullptr;
			int i = 0;
			cJSON_ArrayForEach(json_build_cost_item, json_build_cost) {
				if (json_build_cost_item->type == cJSON_Number) {
					tpl.build_cost[i] = json_build_cost_item->valuedouble;
					i++;
				}
			}
		}

		cJSON* json_upgrade_cost = cJSON_GetObjectItem(json_root, "upgrade_cost");
		if (json_upgrade_cost && json_upgrade_cost->type == cJSON_Array) {
			cJSON* json_upgrade_cost_item = nullptr;
			int i = 0;
			cJSON_ArrayForEach(json_upgrade_cost_item, json_upgrade_cost) {
				if (json_upgrade_cost_item->type == cJSON_Number) {
					tpl.upgrade_cost[i] = json_upgrade_cost_item->valuedouble;
					i++;
				}
			}
		}
	}

	void parse_enemy_template(enemyTemplate& tpl, cJSON* json_root) {
		if (!json_root || json_root->type != cJSON_Object) return;

		cJSON* json_speed = cJSON_GetObjectItem(json_root, "speed");
		if (json_speed && json_speed->type == cJSON_Number) {
			tpl.speed = json_speed->valuedouble;
		}

		cJSON* json_hp = cJSON_GetObjectItem(json_root, "hp");
		if (json_hp && json_hp->type == cJSON_Number) {
			tpl.hp = json_hp->valuedouble;
		}

		cJSON* json_damage = cJSON_GetObjectItem(json_root, "damage");
		if (json_damage && json_damage->type == cJSON_Number) {
			tpl.damage = json_damage->valuedouble;
		}

		cJSON* json_reward_ratio = cJSON_GetObjectItem(json_root, "reward_ratio");
		if (json_reward_ratio && json_reward_ratio->type == cJSON_Number) {
			tpl.reward_ratio = json_reward_ratio->valuedouble;
		}

		cJSON* json_recover_interval = cJSON_GetObjectItem(json_root, "recover_interval");
		if (json_recover_interval && json_recover_interval->type == cJSON_Number) {
			tpl.recover_interval = json_recover_interval->valuedouble;
		}

		cJSON* json_recover_range = cJSON_GetObjectItem(json_root, "recover_range");
		if (json_recover_range && json_recover_range->type == cJSON_Number) {
			tpl.recover_range = json_recover_range->valuedouble;
		}

		cJSON* json_recover_amount = cJSON_GetObjectItem(json_root, "recover_amount");
		if (json_recover_amount && json_recover_amount->type == cJSON_Number) {
			tpl.recover_amount = json_recover_amount->valuedouble;
		}
	}
};	 



#endif // !_CONFIG_MANAGER_H_