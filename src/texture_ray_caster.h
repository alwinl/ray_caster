/*
 * texture_ray_caster.h Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

class TexturePainter : public Game
{
	SetupParams get_params() override { return SetupParams( { "TexturePainter", 640, 480, 0, SDL_RENDERER_ACCELERATED } ); }
	void setup() override {}
	bool process_event( SDL_Event &event ) override { return event.type == SDL_QUIT; }
	void update_state( uint64_t elapsed_time ) override{};
	void draw_frame() override;
};

