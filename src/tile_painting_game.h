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

#ifndef TILEPAINTINGGAME_H
#define TILEPAINTINGGAME_H

#include "sdl2wrapper.h"

#include <vector>

#include <glm/glm.hpp>

#include "player.h"

class TilePaintingGame : public Game
{
public:
	static std::vector<glm::vec4> make_primitive( int total_segments, bool filled );

	static std::vector<glm::vec4> make_rect() { return make_primitive( 4, true ); }
	static std::vector<glm::vec4> make_rect_outline() { return make_primitive( 4, false ); }
	static std::vector<glm::vec4> make_circle( int total_segments ) { return make_primitive( total_segments, true ); };
	static std::vector<glm::vec4> make_circle_outline( int total_segments )
	{
		return make_primitive( total_segments, true );
	};

	void draw_point( glm::vec3 center, float radius, const glm::vec4 colour );
	void draw_line( std::pair<glm::vec3, glm::vec3> points, glm::vec4 color );
	void draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 color );

	bool is_wall( glm::ivec3 cell ) { return level[cell[0] + cell[1] * world_dimension[0]] == '1'; }

private:
	SetupParams get_params() override;
	void setup() override;
	bool process_event( SDL_Event &event ) override;
	void update_state( uint64_t elapsed_time ) override;
	void draw_frame() override;

	void draw_grid();
	void draw_level();

	std::string level = "1111111111"
						"1000100001"
						"1000100001"
						"1000100001"
						"1000000001"
						"1000000001"
						"1000000001"
						"1000001001"
						"1000000001"
						"1111111111";

	glm::ivec2 world_dimension{ 10, 10 };

	const int unit_size = 80;

	Player hero{ { 100, 100 }, unit_size };

	bool quit = false;

	uint8_t KEY_UP = 0;
	uint8_t KEY_DOWN = 0;
	uint8_t KEY_LEFT = 0;
	uint8_t KEY_RIGHT = 0;
	uint8_t KEY_X = 0;
	uint8_t KEY_Z = 0;

	uint8_t KEY_IS_DOWN = 0x01;
};

#endif // TILEPAINTINGGAME_H