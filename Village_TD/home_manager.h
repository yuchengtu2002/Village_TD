#pragma once
#include "manager.h"
#include "config_manager.h"
#include "resource_manager.h"

class HomeManager : public Manager<HomeManager>
{
	friend class Manager<HomeManager>;

public:
	double get_current_hp() {
		return num_hp;
	}

	void decrease_hp(double damage) {
		num_hp -= damage;
		if (num_hp <= 0) {
			num_hp = 0;
			ConfigManager::instance()->is_game_over = true;
			ConfigManager::instance()->is_game_win = false;
		}

		static const ResourcesManager::SoundPool& sound_pool = ResourcesManager::instance()->get_sound_pool();

		Mix_PlayChannel(-1, sound_pool.at(ResID::Sound_HomeHurt), 0);
	}




protected:
	HomeManager() {
		static ConfigManager* config = ConfigManager::instance();
		num_hp = config->initial_hp;

	}
	~HomeManager() = default;

private:
	double num_hp = 0;


}; 