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

Player::Player( TilePaintingGame *game, glm::ivec2 position, int scale_factor )
	: game( game ), position( position, 0.0 ), scale_factor( scale_factor ){};

void Player::move_forward()
{
	const glm::vec3 new_position = get_trans_matrix() * glm::vec4( 0.05F, 0.0F, 0.0F, 1.0F );

	if( !game->is_wall( glm::ivec2( new_position[0] / static_cast<float>( scale_factor ),
									new_position[1] / static_cast<float>( scale_factor ) ) ) )
		position = new_position;
}
void Player::move_back()
{
	const glm::vec3 new_position = get_trans_matrix() * glm::vec4( -0.05F, 0.0F, 0.0F, 1.0F );

	if( !game->is_wall( glm::ivec2( new_position[0] / static_cast<float>( scale_factor ),
									new_position[1] / static_cast<float>( scale_factor ) ) ) )
		position = new_position;
}
void Player::turn_left()
{
	angle -= glm::radians( 1.0 );
}
void Player::turn_right()
{
	angle += glm::radians( 1.0 );
}
void Player::zoom_in()
{
	zoom -= 0.01;
}
void Player::zoom_out()
{
	zoom += 0.01;
}

glm::mat4 Player::get_trans_matrix() const
{
	const auto fscale = static_cast<float>( scale_factor );

	return glm::mat4( {
		{ fscale * std::cos( angle ), fscale * std::sin( angle ), 0.0F, 0.0F },
		{ -fscale * std::sin( angle ), fscale * std::cos( angle ), 0.0F, 0.0F },
		{ 0.0F, 0.0F, 1.0F, 0.0F },
		{ position[0], position[1], 0.0F, 1.0F },
	} );
}