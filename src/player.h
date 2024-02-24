/*
 * player.h Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>

class TilePaintingGame;

class Player
{
public:
	Player( glm::ivec2 position, int scale_factor );

	void draw( TilePaintingGame *game );

	void move_forward();
	void move_back();
	void turn_left();
	void turn_right();
	void zoom_in();
	void zoom_out();

private:
	glm::vec3 position;
	float angle = 0.0;
	float zoom = 1.0;
	int scale_factor;

	glm::mat4 get_trans_matrix();

	void paint_rays( TilePaintingGame *game );
	void paint_direction( TilePaintingGame *game );
	void paint_camera_projection( TilePaintingGame *game );
	void paint_character( TilePaintingGame *game );
};

#endif // PLAYER_H
