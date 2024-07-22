#pragma once

#include "vector2.h"
#include "tile.h"
#include "timer.h"
#include "SDL.h"
#include "resource_manager.h"

class CoinProp {

public:
	CoinProp() {
		timer_jump.set_one_shot(true);
		timer_jump.set_wait_time(interval_jump);
		timer_jump.set_on_timeout([this]() {
			is_jumping = false;
		});

		timer_disappear.set_one_shot(true);
		timer_disappear.set_wait_time(interval_disappear);
		timer_disappear.set_on_timeout([this]() {
			is_valid = false;
		});

		velocity.x = (rand()%2 ? 1:-1) * 2 * TILE_SIZE;
		velocity.y = -3 * TILE_SIZE;

	}
	~CoinProp() = default;

	void set_position(const Vector2& position) {
		this->position = position;
	}

	const Vector2& get_position() const {
		return position;
	}

	const Vector2& get_size() const {
		return size;
	}

	void make_invalid() {
		is_valid = false;
	}

	bool can_remove() const {
		return !is_valid;
	}

	void on_update(double delta) {
		timer_jump.update(delta);
		timer_disappear.update(delta);

		if (is_jumping) {
			position += velocity * delta;
			velocity.y += gravity * delta;
		}
		else {
			velocity.y = 0;
			//mimic coin floating
			velocity.y = sin(SDL_GetTicks64()/1000.0)*30;
		}
		position += velocity * delta;

	}

	void on_render(SDL_Renderer* renderer)
	{
		static SDL_Rect rect = { 0, 0, (int)size.x, (int)size.y };
		static SDL_Texture* tex_coin = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Coin)->second;

		rect.x = (int)(position.x - size.x / 2);
		rect.y = (int)(position.y - size.y / 2);

		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect);
	}




private:
	Vector2 position;
	Vector2 velocity;

	Timer timer_jump;
	Timer timer_disappear;

	bool is_valid = true;
	bool is_jumping = true;

	double gravity = 490;
	double interval_jump = 0.8;
	Vector2 size = Vector2(16, 16);
	double interval_disappear = 10;

};