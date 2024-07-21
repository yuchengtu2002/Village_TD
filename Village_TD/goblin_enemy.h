#pragma once

#include "enemy.h"
#include "config_manager.h"
#include "resource_manager.h"


class GoblinEnemy : public Enemy {
public:
	GoblinEnemy() {
		static ResourcesManager* resource = ResourcesManager::instance();
		static const ResourcesManager::TexturePool& texture_pool = resource->get_texture_pool();
		static SDL_Texture* tex_goblin = texture_pool.find(ResID::Tex_Goblin)->second;
		static SDL_Texture* tex_goblin_sketch = texture_pool.find(ResID::Tex_GoblinSketch)->second;

		static ConfigManager* config = ConfigManager::instance();
		static ConfigManager::enemyTemplate& goblin_template = ConfigManager::instance()->goblin_template;

		static const std::vector<int> idx_list_up = { 6, 7, 8, 9, 10, 11 };
		static const std::vector<int> idx_list_down = { 0, 1, 2, 3, 4, 5 };
		static const std::vector<int> idx_list_right = { 12, 13, 14, 15, 16, 17 };
		static const std::vector<int> idx_list_left = { 11, 12, 13, 14, 15, 16 };

		anim_up.set_loop(true);
		anim_up.set_interval(0.1);
		anim_up.set_frame_data(tex_goblin, 6, 4, idx_list_up);

		anim_down.set_loop(true);
		anim_down.set_interval(0.1);
		anim_down.set_frame_data(tex_goblin, 6, 4, idx_list_down);

		anim_right.set_loop(true);
		anim_right.set_interval(0.1);
		anim_right.set_frame_data(tex_goblin, 6, 4, idx_list_right);

		anim_left.set_loop(true);
		anim_left.set_interval(0.1);
		anim_left.set_frame_data(tex_goblin, 6, 4, idx_list_left);

		anim_up_sketch.set_loop(true);
		anim_up_sketch.set_interval(0.1);
		anim_up_sketch.set_frame_data(tex_goblin_sketch, 6, 4, idx_list_up);

		anim_down_sketch.set_loop(true);
		anim_down_sketch.set_interval(0.1);
		anim_down_sketch.set_frame_data(tex_goblin_sketch, 6, 4, idx_list_down);

		anim_right_sketch.set_loop(true);
		anim_right_sketch.set_interval(0.1);
		anim_right_sketch.set_frame_data(tex_goblin_sketch, 6, 4, idx_list_right);

		anim_left_sketch.set_loop(true);
		anim_left_sketch.set_interval(0.1);
		anim_left_sketch.set_frame_data(tex_goblin_sketch, 6, 4, idx_list_left);

		max_hp = goblin_template.hp;
		max_speed = goblin_template.speed;
		damage = goblin_template.damage;
		reward_ratio = goblin_template.reward_ratio;
		recover_interval = goblin_template.recover_interval;
		recover_range = goblin_template.recover_range;
		recover_amount = goblin_template.recover_amount;

		size.x = 48, size.y = 48;
		hp = max_hp;
		speed = max_speed;
	};

	~GoblinEnemy() = default;
};