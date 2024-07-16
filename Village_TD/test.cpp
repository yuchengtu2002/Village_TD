#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>


using namespace std;

int main1() {

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	Mix_Init(MIX_INIT_MP3);
	TTF_Init();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	SDL_Window* window = SDL_CreateWindow("Village_TD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface* surface = IMG_Load("Assets/avatar.jpg");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	TTF_Font* font = TTF_OpenFont("Assets/ipix.ttf", 24);
	SDL_Color color = color = { 255,255,255,255 };
	SDL_Surface* text_surface = TTF_RenderText_Solid(font, "Hello World", color);
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	Mix_Music* music = Mix_LoadMUS("Assets/music.mp3");
	Mix_FadeInMusic(music, -1, 2000);

	SDL_Rect rect_img, rect_text;
	rect_img.w = surface->w;
	rect_img.h = surface->h;
	rect_text.w = text_surface->w;
	rect_text.h = text_surface->h;

	SDL_Event event;

	SDL_Point pos_cursor = { 0,0 };

	int fps = 60;
	Uint64 last_counter = SDL_GetPerformanceCounter();
	Uint64 counter_freq = SDL_GetPerformanceFrequency();


	bool isRunning = true;

	while (isRunning) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				isRunning = false;
			}
			else if (event.type == SDL_MOUSEMOTION) {
				pos_cursor.x = event.motion.x;
				pos_cursor.y = event.motion.y;
			}
		}

		Uint64 counter = SDL_GetPerformanceCounter();
		float elapsed_time = (counter - last_counter) / (float)counter_freq;
		last_counter = counter;

		if (elapsed_time < 1.0f / fps) {
			SDL_Delay((1.0f / fps - elapsed_time) * 1000);
		}

		rect_img.x = pos_cursor.x;
		rect_img.y = pos_cursor.y;

		rect_text.x = pos_cursor.x;
		rect_text.y = pos_cursor.y;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, &rect_img);
		filledCircleColor(renderer, pos_cursor.x, pos_cursor.y, 50, 0xFF0000FF);
		SDL_RenderCopy(renderer, text_texture, NULL, &rect_text);
		SDL_RenderPresent(renderer);
	}




	return 0;
}