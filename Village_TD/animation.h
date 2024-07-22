#pragma once

#include "timer.h"
#include "SDL.h"
#include <functional>
#include <vector>

class Animation {

public:
	typedef std::function<void()> PlayCallback;

public:
	Animation() {
		timer.set_one_shot(false);
		timer.set_on_timeout([this]() {
			idx_frame++;
			if (idx_frame >= rect_src_list.size()) {
				idx_frame = is_loop ? 0 : rect_src_list.size() - 1;
				if (!is_loop && on_finished) {
					on_finished();
				}
			}
		});
	}

	~Animation() = default;

	void reset() {
		timer.restart();
		idx_frame = 0;
	}

	void set_frame_data(SDL_Texture* texture, int num_h, int num_v, const std::vector<int>& idx_list) {
		int width_tex, height_tex;

		this->texture = texture;

		SDL_QueryTexture(texture, nullptr, nullptr, &width_tex, &height_tex);
		width_frame = width_tex / num_h;
		height_frame = height_tex / num_v;

		rect_src_list.resize(idx_list.size());
		for (size_t i = 0; i < idx_list.size(); i++) {
			int idx = idx_list[i];

			rect_src_list[i] = {
				(idx % num_h) * width_frame,
				(idx / num_h) * height_frame,
				width_frame, height_frame
			};
		}
	}

	void set_loop (bool val) {
		this->is_loop = val;
	}

	void set_interval(double val) {
		timer.set_wait_time(val);
	}

	void set_on_finished(PlayCallback on_finished) {
		this->on_finished = on_finished;
	}

	void on_update(double delta) {
		timer.update(delta);
	}

	void on_render(SDL_Renderer* renderer, const SDL_Point& pos_dst, double angle = 0) const {
		static SDL_Rect rect_dst;
		rect_dst = {
			pos_dst.x, pos_dst.y,
			width_frame, height_frame
		};

		SDL_RenderCopyEx(renderer, texture, &rect_src_list[idx_frame], &rect_dst, angle, nullptr, SDL_FLIP_NONE);
	}

private:
	Timer timer;
	bool is_loop = true;
	size_t idx_frame = 0;
	PlayCallback on_finished;

	SDL_Texture* texture = nullptr;
	std::vector<SDL_Rect> rect_src_list;
	int width_frame = 0, height_frame = 0;


};