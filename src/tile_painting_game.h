/*
 * tile_painting_game.h Copyright 2024 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include "sdl2wrapper.h"

#include <glm/glm.hpp>

class TilePaintingGame : public Game
{
private:
	SetupParams get_params() override;
	void setup() override{};
	bool process_event( SDL_Event &event ) override;
	void update_state( uint64_t elapsed_time ) override;
	void draw_frame() override;

	void paint_floor();
	void paint_ceiling();
	void paint_rays();
	void paint_grid();
	void paint_level();
	void paint_camera();
	void paint_character();

	bool is_wall( glm::ivec2 cell );
	std::pair<int, glm::vec2> calc_intersection( float angle, glm::vec2 ray_start );

	std::string level = "1111111111"
						"1000100001"
						"1000100001"
						"1000100001"
						"1000000001"
						"1000000001"
						"1000000001"
						"1000001001"
						"1000000001"
						"1111011111";

	glm::ivec2 world_dimension{ 10, 10 };

	const int screen_width = 640;
	const int screen_height = 480;

	float unit_size = 10.0F;

	bool quit = false;

	uint8_t KEY_UP = 0;
	uint8_t KEY_DOWN = 1;
	uint8_t KEY_LEFT = 2;
	uint8_t KEY_RIGHT = 3;
	uint8_t KEY_X = 4;
	uint8_t KEY_Z = 5;

	uint8_t key_state = 0;

	glm::vec3 player_position = { 20, 20, 0.0 };
	float player_angle = 0.0;
	float player_zoom = 0.4;

	glm::mat4 player_matrix;
};