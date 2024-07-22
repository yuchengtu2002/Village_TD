#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "manager.h"
#include "config_manager.h"
#include "resource_manager.h"
#include "enemy_manager.h"
#include "home_manager.h"
#include "coin_manager.h"
#include "wave_manager.h"

using namespace std;

class GameManager : public Manager<GameManager> {

	friend class Manager<GameManager>;

public:
	int run() {
		Uint64 LAST = SDL_GetPerformanceCounter();
		const Uint64 FREQUENCY = SDL_GetPerformanceFrequency();

		while (is_running) {
			on_input();

			Uint64 CURRENT = SDL_GetPerformanceCounter();
			double delta = (CURRENT - LAST) / static_cast<double>(FREQUENCY);
			LAST = CURRENT;

			if (delta * 1000 < 16) SDL_Delay(static_cast<Uint32>(16 - delta * 1000));

			on_update(delta);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			on_render();
			SDL_RenderPresent(renderer);
		}
		return 0;
	}

protected:
	GameManager() {
		init_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0, "SDL Initialization Failed");
		init_assert(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG), "SDL Image Initialization Failed");
		init_assert(Mix_Init(MIX_INIT_MP3), "SDL Mixer Initialization Failed");
		init_assert(TTF_Init() == 0, "SDL TTF Initialization Failed");

		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");


		ConfigManager* config = ConfigManager::instance();

		init_assert(config->map.load("map.csv"), "Load Map Failed!");
		init_assert(config->load_level_config("level.json"), "Load Level Config Failed");
		init_assert(config->load_game_config("config.json"), "Load Game Config Failed");


		window = SDL_CreateWindow("Village_TD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
		init_assert(window, "Window Creation Failed");

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		init_assert(renderer, "Renderer Creation Failed");

		init_assert(ResourcesManager::instance()->load_from_file(renderer), u8"加载游戏资源失败！");
		init_assert(generate_tile_map_texture(), u8"生成瓦片地图纹理失败！");


	}

	~GameManager() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		TTF_Quit();
		Mix_Quit();
		IMG_Quit();
		SDL_Quit();
	}

private:
	SDL_Event event;
	bool is_running = true;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* tex_tile_map = nullptr;



private:
	void init_assert(bool flag, const char* err_msg) {
		if (flag) return;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error: Game Start Failed", err_msg, window);
		exit(-1);  // Exit if initialization fails
	}

	void on_input() {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				is_running = false;
			}
		}
	}

	void on_update(double delta) {
		static ConfigManager* config = ConfigManager::instance();
		if (!config->is_game_over) {
			WaveManager::instance()->on_update(delta);
			EnemyManager::instance()->on_update(delta);
		}
	}

	void on_render() {
		static ConfigManager* instance = ConfigManager::instance();
		static SDL_Rect& rect_dst = instance->rect_tile_map;
		SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dst);

		EnemyManager::instance()->on_render(renderer);
	}

	bool generate_tile_map_texture()
	{
		const Map& map = ConfigManager::instance()->map;
		const TileMap& tile_map = map.get_tile_map();
		SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		SDL_Texture* tex_tile_set = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Tileset)->second;

		int width_tex_tile_set, height_tex_tile_set;
		SDL_QueryTexture(tex_tile_set, nullptr, nullptr, &width_tex_tile_set, &height_tex_tile_set);
		int num_tile_single_line = (int)std::ceil((double)width_tex_tile_set / TILE_SIZE);

		int width_tex_tile_map, height_tex_tile_map;
		width_tex_tile_map = (int)map.get_width() * TILE_SIZE;
		height_tex_tile_map = (int)map.get_height() * TILE_SIZE;
		tex_tile_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET, width_tex_tile_map, height_tex_tile_map);
		if (!tex_tile_map) return false;

		ConfigManager* config = ConfigManager::instance();
		rect_tile_map.x = (config->basic_template.window_width - width_tex_tile_map) / 2;
		rect_tile_map.y = (config->basic_template.window_height - height_tex_tile_map) / 2;
		rect_tile_map.w = width_tex_tile_map;
		rect_tile_map.h = height_tex_tile_map;

		SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, tex_tile_map);

		for (int y = 0; y < map.get_height(); y++)
		{
			for (int x = 0; x < map.get_width(); x++)
			{
				SDL_Rect rect_src;
				const Tile& tile = tile_map[y][x];

				const SDL_Rect& rect_dst =
				{
					x * TILE_SIZE, y * TILE_SIZE,
					TILE_SIZE, TILE_SIZE
				};

				rect_src =
				{
					(tile.terrian % num_tile_single_line) * TILE_SIZE,
					(tile.terrian / num_tile_single_line) * TILE_SIZE,
					TILE_SIZE, TILE_SIZE
				};
				SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);

				if (tile.decoration >= 0)
				{
					rect_src =
					{
						(tile.decoration % num_tile_single_line) * TILE_SIZE,
						(tile.decoration / num_tile_single_line) * TILE_SIZE,
						TILE_SIZE, TILE_SIZE
					};
					SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);
				}
			}
		}

		const SDL_Point& idx_home = map.get_home_idx();
		const SDL_Rect rect_dst =
		{
			idx_home.x * TILE_SIZE, idx_home.y * TILE_SIZE,
			TILE_SIZE, TILE_SIZE
		};
		SDL_RenderCopy(renderer, ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Home)->second, nullptr, &rect_dst);

		SDL_SetRenderTarget(renderer, nullptr);

		return true;
	}
};
