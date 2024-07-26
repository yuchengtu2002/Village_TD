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
#include "tower_manager.h"
#include "bullet_manager.h"
#include "status_bar.h"
#include "panel.h"
#include "place_panel.h"
#include "upgrade_panel.h"
#include "player_manager.h"
#include "banner.h"


using namespace std;

class GameManager : public Manager<GameManager> {

	friend class Manager<GameManager>;

public:
	int run() {
		Mix_FadeInMusic(ResourcesManager::instance()->get_music_pool().find(ResID::Music_BGM)->second, -1, 1500);
		TowerManager* tower_manager = TowerManager::instance();

		Uint64 LAST = SDL_GetPerformanceCounter();
		const Uint64 FREQUENCY = SDL_GetPerformanceFrequency();

		while (is_running) {
			while (SDL_PollEvent(&event))
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


    void handle_game_over_input() {
        bool input_handled = false;
        while (!input_handled && SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
                input_handled = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_y) {
                    is_running = true;
                    cout << "received y input" << endl;
                    input_handled = true;
                }
                else if (event.key.keysym.sym == SDLK_n || event.key.keysym.sym == SDLK_x) {
                    is_running = false;
                    input_handled = true;
                }
            }
        }
    }

	void reset_game(){
		ConfigManager* config = ConfigManager::instance();
        config->is_game_over = false;
        config->is_game_win = false;

        EnemyManager::instance()->reset();
        HomeManager::instance()->reset();
        CoinManager::instance()->reset();
        WaveManager::instance()->reset();
        TowerManager::instance()->reset();
        PlayerManager::instance()->reset();

        ConfigManager::instance()->map.load("map.csv");
        generate_tile_map_texture();

        status_bar.set_position(15, 15);
        cout << "Game Reset" << endl;
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

		init_assert(ResourcesManager::instance()->load_from_file(renderer), "Load Resource Failed");
		init_assert(generate_tile_map_texture(), "Generate Tile Map Failed");

		status_bar.set_position(15, 15);

		place_panel = new PlacePanel();
		upgrade_panel = new UpgradePanel();

		banner = new Banner(); 
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
	StatusBar status_bar;

	Panel* place_panel = nullptr;
	Panel* upgrade_panel = nullptr;

	Banner* banner = nullptr;


private:
	void init_assert(bool flag, const char* err_msg) {
		if (flag) return;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error: Game Start Failed", err_msg, window);
		exit(-1);  // Exit if initialization fails
	}

	void on_input()
	{
		static SDL_Point pos_center;
		static SDL_Point idx_tile_selected;
		static ConfigManager* instance = ConfigManager::instance();

		switch (event.type)
		{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (instance->is_game_over)
				break;
			if (get_cursor_idx_tile(idx_tile_selected, event.motion.x, event.motion.y))
			{
				get_selected_tile_center_pos(pos_center, idx_tile_selected);

				if (check_home(idx_tile_selected))
				{
					upgrade_panel->set_idx_tile(idx_tile_selected);
					upgrade_panel->set_center_pos(pos_center);
					upgrade_panel->show();
				}
				else if (can_place_tower(idx_tile_selected))
				{
					place_panel->set_idx_tile(idx_tile_selected);
					place_panel->set_center_pos(pos_center);
					place_panel->show();
				}
			}
			break;
		default:
			break;
		}

		if (!instance->is_game_over)
		{
			place_panel->on_input(event);
			upgrade_panel->on_input(event);
			PlayerManager::instance()->on_input(event);
		}
	}


	void on_update(double delta) {
		static bool is_game_over_last_tick = false;

		static ConfigManager* config = ConfigManager::instance();
		if (!config->is_game_over) {
			status_bar.on_update(renderer);
			WaveManager::instance()->on_update(delta);
			EnemyManager::instance()->on_update(delta);
			BulletManager::instance()->on_update(delta);
			TowerManager::instance()->on_update(delta);
			PlayerManager::instance()->on_update(delta); 
			place_panel->on_update(renderer);
			upgrade_panel->on_update(renderer);
			return;
		}

		if (!is_game_over_last_tick && config->is_game_over) {
				static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::instance()->get_sound_pool();

				Mix_FadeOutMusic(1500);
				Mix_PlayChannel(-1, sound_pool.find(config->is_game_win ? ResID::Sound_Win : ResID::Sound_Loss)->second, 0);
			}

		is_game_over_last_tick = config->is_game_over;

		banner->on_update(delta);
		if (banner->is_end()) {
			handle_game_over_input();
		}

		if (is_running) {
			reset_game();
		}
	}

	void on_render() {
		static ConfigManager* instance = ConfigManager::instance();
		static SDL_Rect& rect_dst = instance->rect_tile_map;
		SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dst);

		EnemyManager::instance()->on_render(renderer);
		BulletManager::instance()->on_render(renderer);
		TowerManager::instance()->on_render(renderer);
		PlayerManager::instance()->on_render(renderer);

		if (!instance->is_game_over) {
			place_panel->on_render(renderer); 
			upgrade_panel->on_render(renderer); 
			status_bar.on_render(renderer);
			return;
		}

		int width_screen, height_screen;
		SDL_GetWindowSizeInPixels(window, &width_screen, &height_screen);
		Vector2 centrePos = { (double)width_screen / 2, (double)height_screen / 2 };
		banner->set_centre_position(centrePos);
		banner->on_render(renderer);
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

	bool get_cursor_idx_tile(SDL_Point& idx_tile_selected, int screen_x, int screen_y) const {
		static const Map& map = ConfigManager::instance()->map;
		static SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;

		if (screen_x < rect_tile_map.x || screen_x >= rect_tile_map.x + rect_tile_map.w ||
			screen_y < rect_tile_map.y || screen_y >= rect_tile_map.y + rect_tile_map.h) {
			return false;
		}

		idx_tile_selected.x = std::min((screen_x - rect_tile_map.x) / TILE_SIZE, (int)map.get_width()-1);
		idx_tile_selected.y = std::min((screen_y - rect_tile_map.y) / TILE_SIZE, (int)map.get_height()-1);

		return true;
	}

	bool can_place_tower(const SDL_Point& idx_tile_selected) {
		static const Map& map = ConfigManager::instance()->map;
		const Tile& tile = map.get_tile_map()[idx_tile_selected.y][idx_tile_selected.x];

		return (tile.decoration < 0 && tile.direction == Tile::Direction::None && !tile.has_tower);
	}

	void get_selected_tile_center_pos(SDL_Point& pos, const SDL_Point& idx_tile_selected) const {
		static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		pos.x = rect_tile_map.x + idx_tile_selected.x * TILE_SIZE + TILE_SIZE / 2;
		pos.y = rect_tile_map.y + idx_tile_selected.y * TILE_SIZE + TILE_SIZE / 2;
	}

	bool check_home(const SDL_Point& idx_tile_selected) {
		static const Map& map = ConfigManager::instance()->map;
		const SDL_Point& idx_home = map.get_home_idx();
		return (idx_tile_selected.x == idx_home.x && idx_tile_selected.y == idx_home.y);
	}


};
