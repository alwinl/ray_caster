/*
 * player.cc Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

#include "player.h"

#include "tile_painting_game.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player( glm::ivec2 position, int scale_factor ) : position( position, 0.0 ), scale_factor( scale_factor ){};

void Player::paint_rays( TilePaintingGame *game ){};

void Player::move_forward()
{
	position = get_trans_matrix() * glm::vec4( 0.05F, 0.0F, 0.0F, 1.0F );
}
void Player::move_back()
{
	position = get_trans_matrix() * glm::vec4( -0.05F, 0.0F, 0.0F, 1.0F );
}
void Player::turn_left()
{
	angle -= 1.0;
}
void Player::turn_right()
{
	angle += 1.0;
}
void Player::zoom_in()
{
	zoom -= 0.01;
}
void Player::zoom_out()
{
	zoom += 0.01;
}

glm::mat4 Player::get_trans_matrix()
{
	glm::mat4 trans = glm::mat4( 1.0F );
	trans = glm::translate( trans, position );
	trans = glm::rotate( trans, glm::radians( angle ), glm::vec3( 0.0F, 0.0F, 1.0F ) );
	trans = glm::scale( trans, glm::vec3( scale_factor, scale_factor, 1 ) );

	return trans;
}

void Player::draw( TilePaintingGame *game )
{
	const glm::mat4 trans = get_trans_matrix();

	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	const glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );

	const glm::vec4 dir = trans * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 cam_left = trans * glm::vec4( 1.0F, zoom, 0.0F, 1.0F );
	const glm::vec4 cam_right = trans * glm::vec4( 1.0F, -zoom, 0.0F, 1.0F );
	const glm::vec4 left_ray = trans * glm::vec4( 2.0F, 2 * zoom, 0.0F, 1.0F );
	const glm::vec4 right_ray = trans * glm::vec4( 2.0F, 2 * -zoom, 0.0F, 1.0F );

	game->draw_line( position, dir, red );
	game->draw_line( cam_left, cam_right, green );
	game->draw_line( position, left_ray, blue );
	game->draw_line( position, right_ray, blue );

	const glm::vec4 yellow = glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	glm::mat4 circle_rot = trans;
	circle_rot = glm::rotate( trans, glm::radians( 45.0F ), glm::vec3( 0.0, 0.0, 1.0 ) );
	circle_rot = glm::scale( circle_rot, glm::vec3( 0.1 ) );

	std::vector<glm::vec4> circle_verts = TilePaintingGame::make_circle( static_cast<int>( 32 ) );
	std::transform( circle_verts.begin(), circle_verts.end(), circle_verts.begin(),
					[&circle_rot]( glm::vec4 point ) { return circle_rot * point; } );

	game->draw_geometry( circle_verts, yellow );

	paint_rays( game );
}