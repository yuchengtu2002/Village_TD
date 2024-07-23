#pragma once

#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "resource_manager.h"
#include "coin_manager.h"
#include "home_manager.h"
#include "player_manager.h"
#include <string>

class StatusBar {

public:
	StatusBar() = default;
	~StatusBar() = default;

	void set_position(int x, int y) {
		position.x = x, position.y = y;
	}

	void on_update(SDL_Renderer* renderer) {
		static TTF_Font* font = ResourcesManager::instance()->get_font_pool().find(ResID::Font_Main)->second;

		SDL_DestroyTexture(tex_text_bg);
		tex_text_bg = nullptr;
		SDL_DestroyTexture(tex_text_fg);
		tex_text_fg = nullptr;

		std::string str_val = std::to_string((int)CoinManager::instance()->get_current_coin());
		SDL_Surface* surf_text_bg = TTF_RenderText_Blended(font, str_val.c_str(), color_text_bg);
		SDL_Surface* surf_text_fg = TTF_RenderText_Blended(font, str_val.c_str(), color_text_fg);

		width_text = surf_text_bg->w; height_text = surf_text_bg->h;

		tex_text_bg = SDL_CreateTextureFromSurface(renderer, surf_text_bg);
		tex_text_fg = SDL_CreateTextureFromSurface(renderer, surf_text_fg);

		SDL_FreeSurface(surf_text_bg);
		SDL_FreeSurface(surf_text_fg);
	}

	void on_render(SDL_Renderer* renderer) {
		static SDL_Rect rect_dst;
		static const ResourcesManager::TexturePool& tex_pool = ResourcesManager::instance()->get_texture_pool();
		static SDL_Texture* tex_heart = tex_pool.find(ResID::Tex_UIHeart)->second;
		static SDL_Texture* tex_coin = tex_pool.find(ResID::Tex_UICoin)->second;
		static SDL_Texture* tex_home = tex_pool.find(ResID::Tex_UIHomeAvatar)->second;
		static SDL_Texture* tex_player = tex_pool.find(ResID::Tex_UIPlayerAvatar)->second;

		//home avatar
		rect_dst.x = position.x, rect_dst.y = position.y;
		rect_dst.w = 78, rect_dst.h = 78;
		SDL_RenderCopy(renderer, tex_home, nullptr, &rect_dst);

		//heart avatar
		int offset = 2;
		for (int i = 0; i < (int)HomeManager::instance()->get_current_hp(); i++) {
			rect_dst.x = position.x + 78 + 5 + i * (size_heart + offset), rect_dst.y = position.y;
			rect_dst.w = size_heart, rect_dst.h = size_heart;
			SDL_RenderCopy(renderer, tex_heart, nullptr, &rect_dst);
		}

		//Coin avatar
		rect_dst.x = position.x + 78 + 15;
		rect_dst.y = position.y + 78 - 32;
		rect_dst.w = 32, rect_dst.h = 32;
		SDL_RenderCopy(renderer, tex_coin, nullptr, &rect_dst);

		//Coin text with shadow
		rect_dst.x += 32 + 8 + offset_shadow.x;
		rect_dst.y = rect_dst.y + (32 - height_text) / 2 + offset_shadow.y;
		rect_dst.w = width_text, rect_dst.h = height_text;
		SDL_RenderCopy(renderer, tex_text_bg, nullptr, &rect_dst);

		rect_dst.x -= offset_shadow.x;
		rect_dst.y -= offset_shadow.y;
		SDL_RenderCopy(renderer, tex_text_fg, nullptr, &rect_dst);

		//dragon avatar
		rect_dst.x = position.x + (78 - 65) / 2;
		rect_dst.y = position.y + 78 + 10;
		rect_dst.w = 65, rect_dst.h = 65;
		SDL_RenderCopy(renderer, tex_player, nullptr, &rect_dst);

		//MP bar background
		rect_dst.x = position.x + 78 + 15;
		rect_dst.y = position.y + 78 + 20; // Adjusted to ensure it's below the previous elements
		roundedBoxRGBA(renderer, rect_dst.x, rect_dst.y, rect_dst.x + width_mp_bar, rect_dst.y + height_mp_bar, 4,
			color_mp_bar_background.r, color_mp_bar_background.g, color_mp_bar_background.b, color_mp_bar_background.a);

		//MP bar foreground
		rect_dst.x += width_border_mp_bar;
		rect_dst.y += width_border_mp_bar;
		rect_dst.w = width_mp_bar - 2 * width_border_mp_bar;
		rect_dst.h = height_mp_bar - 2 * width_border_mp_bar;
		double mp_ratio = PlayerManager::instance()->get_current_mp() / 100;
		roundedBoxRGBA(renderer, rect_dst.x, rect_dst.y, rect_dst.x + (int)(rect_dst.w * mp_ratio), rect_dst.y + rect_dst.h, 2,
			color_mp_bar_foreground.r, color_mp_bar_foreground.g, color_mp_bar_foreground.b, color_mp_bar_foreground.a);
	}


private:
	const int size_heart = 32;
	const int width_mp_bar = 200;
	const int height_mp_bar = 20;

	const int width_border_mp_bar = 4;
	const SDL_Point offset_shadow = { 2, 2 };

	const SDL_Color color_text_bg = { 175, 175, 175, 255 };
	const SDL_Color color_text_fg = { 255, 255, 255, 255 };

	const SDL_Color color_mp_bar_background = { 48, 40, 51, 255 };
	const SDL_Color color_mp_bar_foreground = { 144, 121, 173, 255 };

private:
	SDL_Point position = { 0 };
	int width_text = 0, height_text = 0;
	SDL_Texture* tex_text_bg = nullptr;
	SDL_Texture* tex_text_fg = nullptr;
};