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

void Player::draw()
{
	paint_rays();
	// paint_direction();
	paint_camera_projection();
	paint_character();
}

void Player::paint_rays()
{
	glm::vec4 grey = glm::vec4( 0.75F, 0.75F, 0.75F, 1.0F );
	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 magenta = glm::vec4( 0.0F, 1.0F, 1.0F, 1.0F );

	const int resolution = scale_factor * 10; // screen_width

	float rot_angle = glm::radians( angle ) - std::atan( zoom );
	const float delta_angle = 2.0F * std::atan( zoom ) / ( static_cast<float>( resolution ) );

	for( int step = 0; step <= resolution; ++step ) {

		const std::pair<int, glm::vec2> result = calc_intersection( rot_angle );
		const glm::vec3 intersection( result.second * scale_factor, 0.0 );

		if( result.first != -1 ) {
			game->draw_point( intersection, 3, red );
			game->draw_line( std::pair<glm::vec3, glm::vec3>( position, intersection ), magenta );

			if( result.first == 0 )
				grey = glm::vec4( 0.5F, 0.5F, 0.5F, 1.0F );
			else
				grey = glm::vec4( 0.75F, 0.75F, 0.75F, 1.0F );

			glm::vec2 delta( intersection[0] - position[0], intersection[1] - position[1] );

			// https://www.youtube.com/watch?v=eOCQfxRQ2pY
			const float distance = delta[0] * cos( glm::radians( angle ) ) + delta[1] * sin( glm::radians( angle ) );

			const float height = static_cast<float>( scale_factor ) * 400.0F / distance;
			const std::pair<glm::vec3, glm::vec3> points = {
				glm::vec3( 10 * scale_factor + step, 5.0 * scale_factor - height, 0 ),
				glm::vec3( 10 * scale_factor + step, 5.0 * scale_factor + height, 0 ) };

			game->draw_line( points, grey );
		}

		rot_angle += delta_angle;
	}
}

// https://www.youtube.com/watch?v=NbSee-XM7WA
std::pair<int, glm::vec2> Player::calc_intersection( float angle )
{
	glm::vec2 vRayStart = glm::vec2( position[0], position[1] ) / ( 1.0 * scale_factor );
	glm::vec2 vRayDir = { std::cos( angle ), std::sin( angle ) };
	glm::vec2 vRayUnitStepSize = { std::sqrt( 1 + ( vRayDir[1] / vRayDir[0] ) * ( vRayDir[1] / vRayDir[0] ) ),
								   std::sqrt( 1 + ( vRayDir[0] / vRayDir[1] ) * ( vRayDir[0] / vRayDir[1] ) ) };

	glm::ivec2 vMapCheck( vRayStart );
	glm::vec2 vRayLength1D = {
		( vRayDir[0] < 0 ) ? ( vRayStart[0] - 1.0 * vMapCheck[0] ) * vRayUnitStepSize[0] :
							 ( 1.0 - ( vRayStart[0] - 1.0 * vMapCheck[0] ) ) * vRayUnitStepSize[0],
		( vRayDir[1] < 0 ) ? ( vRayStart[1] - 1.0 * vMapCheck[1] ) * vRayUnitStepSize[1] :
							 ( 1.0 - ( vRayStart[1] - 1.0 * vMapCheck[1] ) ) * vRayUnitStepSize[1] };

	glm::ivec2 vStep = { ( vRayDir[0] < 0 ) ? -1 : 1, ( vRayDir[1] < 0 ) ? -1 : 1 };

	bool bTileFound = false;
	const float fMaxDistance = 100.0;
	float fDistance = 0.0;
	int walk_side = -1;

	while( !bTileFound && fDistance < fMaxDistance ) {

		walk_side = ( vRayLength1D[0] < vRayLength1D[1] ) ? 0 : 1;

		vMapCheck[walk_side] += vStep[walk_side];
		fDistance = vRayLength1D[walk_side];
		vRayLength1D[walk_side] += vRayUnitStepSize[walk_side];

		bTileFound = game->is_wall( vMapCheck );
	}

	if( !bTileFound )
		return std::make_pair( -1, glm::vec2() );

	return std::make_pair( walk_side, glm::vec2( { vRayStart + vRayDir * fDistance } ) );
}

void Player::paint_direction()
{
	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 dir = get_trans_matrix() * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );

	game->draw_line( std::pair<glm::vec3, glm::vec3>( position, dir ), red );
}

void Player::paint_camera_projection()
{
	const glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	const glm::vec4 cam_left = get_trans_matrix() * glm::vec4( 1.0F, -zoom, 0.0F, 1.0F );
	const glm::vec4 cam_right = get_trans_matrix() * glm::vec4( 1.0F, zoom, 0.0F, 1.0F );

	game->draw_line( std::pair<glm::vec3, glm::vec3>( cam_left, cam_right ), green );
}

void Player::paint_character()
{
	const glm::vec4 yellow = glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	game->draw_point( position, 6.0, yellow );
}