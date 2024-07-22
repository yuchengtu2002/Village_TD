#pragma once

#include "vector2.h"
#include "animation.h"
#include "config_manager.h"
#include "enemy.h"

class Bullet {

public:
	Bullet() {}

	~Bullet() {

	}

	void set_velocity(const Vector2& v) {
		this->velocity = v;

		if (can_rotate) {
			angle_anim_rotated = atan2(v.y, v.x) * 180 / M_PI;
		}
	}

	void set_position(const Vector2& p) {
		this->position = p;
	}


	void set_damange(double damange) {
		this->damage = damange;
	}

	const Vector2& get_position() const {
		return position;
	}

	double get_damage() const {
		return damage;
	}

	double get_damage_range() const {
		return damage_range;
	}

	void disable_collide() {
		is_collisional = false;
	}

	bool can_collide() const{
		return is_collisional;
	}

	void make_invalid() {
		is_valid = false;
		is_collisional = false;
	}

	bool can_remove() const {
		return !is_valid;
	}

	virtual void on_update(double delta) {
		position += velocity * delta;
		animation.on_update(delta);

		static const SDL_Rect rect_map = ConfigManager::instance()->rect_tile_map;
		if (position.x - size.x / 2 < rect_map.x || position.x + size.x / 2 > rect_map.x + rect_map.w ||
			position.y - size.y / 2 < rect_map.y || position.y + size.y / 2 > rect_map.y + rect_map.h) {
			make_invalid();
		}
	}

	virtual void on_render(SDL_Renderer* renderer) {
		static SDL_Point point;

		point.x = (int)(position.x - size.x / 2);
		point.y = (int)(position.y - size.y / 2);

		animation.on_render(renderer, point, angle_anim_rotated);

	}

	virtual void on_collide(Enemy* enemy) {
		is_valid = false;
		is_collisional = false;
	}



protected:
	Vector2 size;
	Vector2 position;
	Vector2 velocity;

	Animation animation;
	bool can_rotate = false;

	double damage = 0;
	double damage_range = -1;



private:
	bool is_valid = true;
	bool is_collisional = true;

	double angle_anim_rotated = 0;




};