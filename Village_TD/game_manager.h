#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "manager.h"

class GameManager : public Manager<GameManager>{
public:
	friend class Manager<GameManager>;

	
protected:
	GameManager() {
		init_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0, "SDL Initialization Failed");
		init_assert(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG), "SDL Image Initialization Failed");
		init_assert(Mix_Init(MIX_INIT_MP3), "SDL Mixer Initialization Failed");
		init_assert(TTF_Init() == 0, "SDL TTF Initialization Failed");

		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

		window = SDL_CreateWindow("Village_TD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
		init_assert(window, "Window Creation Failed");

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		init_assert(renderer, "Renderer Creation Failed");
	}

	~GameManager() = default;

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

private: 
	void init_assert(bool flag, const char* err_msg) {
		if (flag) return;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error: Game Start Failed", err_msg, NULL);
	}
};
