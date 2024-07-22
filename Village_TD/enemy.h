#pragma once

#include "vector2.h"
#include "timer.h"
#include "animation.h"
#include <functional>
#include "Route.h"
#include "config_manager.h"
#include "SDL.h"

class Enemy {

public:
	typedef std::function<void(Enemy* enemy)> SkillCallback;

public:
	Enemy() {
		timer_skill.set_one_shot(false);
		timer_skill.set_on_timeout([this]() {
			on_skill_released(this);
		});

		timer_sketch.set_one_shot(true);
		timer_sketch.set_wait_time(0.1);
		timer_sketch.set_on_timeout([this]() {
			is_sketch = false;
		});

		timer_restore_speed.set_one_shot(true);
		timer_restore_speed.set_on_timeout([this]() {
			speed = max_speed;
		});



	};

	~Enemy() = default;

	void on_update(double delta) {
		timer_skill.update(delta);
		timer_sketch.update(delta);
		timer_restore_speed.update(delta);

		Vector2 move_distance = velocity * delta;
		Vector2 target_distance = position_target - position;
		position += move_distance < target_distance ? move_distance : target_distance;

		if (target_distance.approx_zero()) {
			idx_target++;
			refresh_position_target();

			direction = (position_target - position).normalize();
		}
		velocity.x = direction.x * speed * TILE_SIZE;
		velocity.y = direction.y * speed * TILE_SIZE;

		bool is_show_x_amin = abs(velocity.x) > abs(velocity.y);

		if (is_sketch) {
			if (is_show_x_amin) {
				anim_current = velocity.x > 0 ? &anim_right_sketch : &anim_left_sketch;
			}
			else {
				anim_current = velocity.y > 0 ? &anim_down_sketch : &anim_up_sketch;
			}
		}
		else {
			if (is_show_x_amin) {
				anim_current = velocity.x > 0 ? &anim_right : &anim_left;
			}
			else {
				anim_current = velocity.y > 0 ? &anim_down : &anim_up;
			}
		}
		anim_current->on_update(delta);
	}

	void on_render(SDL_Renderer* renderer) {
		static SDL_Point point;
		point.x = (int)(position.x - size.x/2);
		point.y = (int)(position.y - size.y/2);
		anim_current->on_render(renderer, point);

		static SDL_Rect rect;
		static const Vector2 size_hp_bar{ 40, 8 };
		static int offset_y = 2;
		static const SDL_Color color_boarder = { 116, 185, 124, 255 };
		static const SDL_Color color_hp = { 226, 255, 194, 255 };

		if (hp < max_hp) {
			rect.x = (int)(position.x - size_hp_bar.x / 2);
			rect.y = (int)(position.y - size.y / 2 - size_hp_bar.y - offset_y);
			rect.w = (int)(size_hp_bar.x * hp / max_hp);
			rect.h = (int)size_hp_bar.y;
			SDL_SetRenderDrawColor(renderer, color_boarder.r, color_boarder.g, color_boarder.b, color_boarder.a);
			SDL_RenderDrawRect(renderer, &rect);

			rect.w = (int)size_hp_bar.x;
			SDL_SetRenderDrawColor(renderer, color_hp.r, color_hp.g, color_hp.b, color_hp.a);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	void set_on_skill_released(SkillCallback on_skill_released) {
		this->on_skill_released = on_skill_released;
	}

	void recover_hp(double amount) {
		hp += amount;
		if (hp > max_hp) {
			hp = max_hp;
		}
	}

	void decrease_hp(double amount) {
		hp -= amount;
		if (hp <= 0) {
			hp = 0;
			is_valid = false;
		}
		is_sketch = true;
		timer_sketch.restart();
	}

	void slow_down(double ratio, double duration) {
		speed *= ratio;
		timer_restore_speed.set_wait_time(duration);
		timer_restore_speed.restart();
	}

	void set_position(const Vector2& position) {
		this->position = position;
	}

	void set_route(const Route* route) {
		this->route = route;
		refresh_position_target();
	}

	void make_invalid() {
		is_valid = false;
	}

	double get_hp() const {
		return hp;
	}

	const Vector2& get_size() const {
		return size;
	}

	const Vector2& get_position() const {
		return position;
	}

	const Vector2& get_velocity() const {
		return velocity;
	}

	double get_damage() const {
		return damage;
	}

	double get_reward_ratio() const {
		return reward_ratio;
	}

	double get_recover_range() const {
		return recover_range * TILE_SIZE;
	}

	double get_recover_amount() const {
		return recover_amount;
	}

	bool can_remove() const{
		return !is_valid;
	}

	double get_route_process() const {
		if (route->get_idx_list().size() == 1) return 1;
		return (double)idx_target / (route->get_idx_list().size()-1);
	}

protected:
	Vector2 size;

	Timer timer_skill;

	Animation anim_up;
	Animation anim_down;
	Animation anim_left;
	Animation anim_right;
	Animation anim_up_sketch;
	Animation anim_down_sketch;
	Animation anim_left_sketch;
	Animation anim_right_sketch;

	double hp = 0;
	double max_hp = 0;
	double speed = 0;
	double max_speed = 0;
	double damage = 0;
	double reward_ratio = 0;
	double recover_interval = 0;
	double recover_range = 0;
	double recover_amount = 0;

private:
	Vector2 position;
	Vector2 velocity;
	Vector2 direction;

	bool is_valid = true;
	
	Timer timer_sketch;
	bool is_sketch = false;

	Animation* anim_current = nullptr;

	SkillCallback on_skill_released = nullptr;
	Timer timer_restore_speed;

	const Route* route = nullptr;
	int idx_target = 0;

	Vector2 position_target;

private:
	void refresh_position_target() {
		const Route::IdxList& idx_list = route->get_idx_list();

		if (idx_target < idx_list.size()) {
			const SDL_Point& point = idx_list[idx_target];
			static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

			position_target = {
				(double)(rect_tile_map.x+ point.x * TILE_SIZE + TILE_SIZE / 2),
				(double)(rect_tile_map.y + point.y * TILE_SIZE + TILE_SIZE / 2)

			};
		}

	}

};