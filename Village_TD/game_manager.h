#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "manager.h"

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

		window = SDL_CreateWindow("Village_TD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
		init_assert(window, "Window Creation Failed");

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		init_assert(renderer, "Renderer Creation Failed");
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
		// Update game logic here
	}

	void on_render() {
		// Render game objects here
	}
};
