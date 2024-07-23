#pragma once

#include "panel.h"
#include "tower_manager.h"
#include "resource_manager.h"

class UpgradePanel : public Panel {
public:
	UpgradePanel() {
		const ResourcesManager::TexturePool& tex_pool = ResourcesManager::instance()->get_texture_pool();

		tex_idle = tex_pool.find(ResID::Tex_UIUpgradeIdle)->second;
		tex_hovered_top = tex_pool.find(ResID::Tex_UIUpgradeHoveredTop)->second;
		tex_hovered_left = tex_pool.find(ResID::Tex_UIUpgradeHoveredLeft)->second;
		tex_hovered_right = tex_pool.find(ResID::Tex_UIUpgradeHoveredRight)->second;
	}

	~UpgradePanel() {
		SDL_DestroyTexture(tex_idle);
		SDL_DestroyTexture(tex_hovered_top);
		SDL_DestroyTexture(tex_hovered_left);
		SDL_DestroyTexture(tex_hovered_right);
	}

	void on_update(SDL_Renderer* renderer) {
		static TowerManager* instance = TowerManager::instance();
		val_top = (int)instance->get_upgrade_cost(TowerType::Axeman);
		val_left = (int)instance->get_upgrade_cost(TowerType::Archer);
		val_right = (int)instance->get_upgrade_cost(TowerType::Gunner);

		Panel::on_render(renderer);
	}

};