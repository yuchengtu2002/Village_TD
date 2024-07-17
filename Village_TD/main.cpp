#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include "Manager.h"
#include "game_manager.h"


using namespace std;

int main() {

	GameManager* instance = GameManager::instance();



	return 0;
}