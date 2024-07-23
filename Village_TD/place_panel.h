#pragma once

#include "panel.h"
#include "resource_manager.h"
#include "tower_manager.h"
#include "SDL2_gfxPrimitives.h"
#include "coin_manager.h"

class PlacePanel : public Panel {
public: 
	PlacePanel() {
		const ResourcesManager::TexturePool& tex_pool = ResourcesManager::instance()->get_texture_pool();

		tex_idle = tex_pool.find(ResID::Tex_UIPlaceIdle)->second;
		tex_hovered_top = tex_pool.find(ResID::Tex_UIPlaceHoveredTop)->second;
		tex_hovered_left = tex_pool.find(ResID::Tex_UIPlaceHoveredLeft)->second;
		tex_hovered_right = tex_pool.find(ResID::Tex_UIPlaceHoveredRight)->second;
	}

	~PlacePanel() {
		SDL_DestroyTexture(tex_idle);
		SDL_DestroyTexture(tex_hovered_top);
		SDL_DestroyTexture(tex_hovered_left);
		SDL_DestroyTexture(tex_hovered_right);
	}

public:
	void on_update(SDL_Renderer* renderer) override {
		static TowerManager* instance = TowerManager::instance();
		val_top = (int)instance->get_place_cost(TowerType::Axeman);
		val_left = (int)instance->get_place_cost(TowerType::Archer);
		val_right = (int)instance->get_place_cost(TowerType::Gunner);

		reg_top = (int)instance->get_damage_range(TowerType::Axeman) * TILE_SIZE;
		reg_left = (int)instance->get_damage_range(TowerType::Archer) * TILE_SIZE;
		reg_right = (int)instance->get_damage_range(TowerType::Gunner) * TILE_SIZE;

		Panel::on_update(renderer);
	}
		
	void on_render(SDL_Renderer* renderer) override {
		if (!visible) return;
		int reg = 0;

		switch (hovered_target)
		{
			case HoveredTarget::Top:
				reg = reg_top;
				break;
			case HoveredTarget::Left:
				reg = reg_left;
				break;
			case HoveredTarget::Right:
				reg = reg_right;
				break;
		}

		if (reg > 0) {
			filledCircleRGBA(renderer, center_pos.x, center_pos.y, reg, color_region_content.r, color_region_content.g, color_region_content.b, color_region_content.a);
			aacircleRGBA(renderer, center_pos.x, center_pos.y, reg, color_region_frame.r, color_region_frame.g, color_region_frame.b, color_region_frame.a);
		}

		Panel::on_render(renderer);

	}

protected:
	void on_click_top_area() override
	{
		CoinManager* instance = CoinManager::instance();

		if (val_top <= instance->get_current_coin())
		{
			TowerManager::instance()->place_tower(TowerType::Axeman, idx_tile_selected);
			instance->decrease_coin(val_top);
		}
	}

	void on_click_left_area() override
	{
		CoinManager* instance = CoinManager::instance();

		if (val_left <= instance->get_current_coin())
		{
			TowerManager::instance()->place_tower(TowerType::Archer, idx_tile_selected);
			instance->decrease_coin(val_left);
		}
	}

	void on_click_right_area() override
	{
		CoinManager* instance = CoinManager::instance();

		if (val_right <= instance->get_current_coin())
		{
			TowerManager::instance()->place_tower(TowerType::Gunner, idx_tile_selected);
			instance->decrease_coin(val_right);
		}
	}

private:
	const SDL_Color color_region_frame = { 30, 80, 162, 175 };
	const SDL_Color color_region_content = { 0, 149, 217, 100 };
private:
	int reg_top = 0, reg_left = 0, reg_right = 0;

};