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

glm::mat4 Player::get_trans_matrix()
{
	const auto fscale = static_cast<float>( scale_factor );

	return glm::mat4( {
		{ fscale * std::cos( angle ), fscale * std::sin( angle ), 0.0F, 0.0F },
		{ -fscale * std::sin( angle ), fscale * std::cos( angle ), 0.0F, 0.0F },
		{ 0.0F, 0.0F, 1.0F, 0.0F },
		{ position[0], position[1], 0.0F, 1.0F },
	} );
}

void Player::draw()
{
	paint_rays();

	// minimap
	paint_direction();
	paint_camera_projection();
	paint_character();
}

void Player::paint_rays()
{
	constexpr int x_dim = 0;
	constexpr int y_dim = 1;

	constexpr glm::vec4 dark_grey = glm::vec4( 0.75F, 0.75F, 0.75F, 1.0F );
	constexpr glm::vec4 light_grey = glm::vec4( 0.5F, 0.5F, 0.5F, 1.0F );

	const int resolution = scale_factor * 10; // screen_width

	float rot_angle = angle - std::atan( zoom );
	const float delta_angle = 2.0F * std::atan( zoom ) / ( static_cast<float>( resolution ) );

	for( int step = 0; step <= resolution; ++step ) {

		const std::pair<int, glm::vec2> result = calc_intersection( rot_angle );
		const glm::vec3 intersection( result.second * scale_factor, 0.0 );
		const int wall_side = result.first;

		if( wall_side != -1 ) {

			glm::vec2 delta( intersection[x_dim] - position[x_dim], intersection[y_dim] - position[y_dim] );

			// credit: https://www.youtube.com/watch?v=eOCQfxRQ2pY
			const float distance = delta[x_dim] * cos( angle ) + delta[y_dim] * sin( angle );

			const float height = static_cast<float>( scale_factor ) * 400.0F / distance;
			const std::pair<glm::vec3, glm::vec3> points = {
				glm::vec3( 10 * scale_factor + step, 5.0 * scale_factor - height, 0 ),
				glm::vec3( 10 * scale_factor + step, 5.0 * scale_factor + height, 0 ) };

			game->draw_line( points, ( wall_side == x_dim ) ? light_grey : dark_grey );
		}

		rot_angle += delta_angle;
	}
}

// adapted based on https://www.youtube.com/watch?v=NbSee-XM7WA
std::pair<int, glm::vec2> Player::calc_intersection( float angle )
{
	constexpr int x_dim = 0;
	constexpr int y_dim = 1;

	glm::vec2 ray_start = glm::vec2( position[x_dim], position[y_dim] ) / ( 1.0 * scale_factor );
	glm::vec2 ray_dir = { std::cos( angle ), std::sin( angle ) };
	glm::vec2 unit_step_size = {
		std::sqrt( 1 + ( ray_dir[y_dim] / ray_dir[x_dim] ) * ( ray_dir[y_dim] / ray_dir[x_dim] ) ),
		std::sqrt( 1 + ( ray_dir[x_dim] / ray_dir[y_dim] ) * ( ray_dir[x_dim] / ray_dir[y_dim] ) ) };

	glm::ivec2 cell_to_test( ray_start );

	glm::vec2 ray_length_by_dimension;
	glm::ivec2 step;

	for( const int dim : { x_dim, y_dim } ) {

		const float offset = ray_start[dim] - static_cast<float>( cell_to_test[dim] );

		if( ray_dir[dim] < 0 ) {
			step[dim] = -1;
			ray_length_by_dimension[dim] = offset * unit_step_size[dim];
		} else {
			step[dim] = 1;
			ray_length_by_dimension[dim] = ( 1.0F - offset ) * unit_step_size[dim];
		}
	}

	constexpr float max_distance = 100.0;
	float current_side_distance = 0.0;
	bool wall_found = false;
	int walk_side = -1;

	while( !wall_found && current_side_distance < max_distance ) {

		walk_side = ( ray_length_by_dimension[x_dim] < ray_length_by_dimension[y_dim] ) ? x_dim : y_dim;

		cell_to_test[walk_side] += step[walk_side];
		current_side_distance = ray_length_by_dimension[walk_side];
		ray_length_by_dimension[walk_side] += unit_step_size[walk_side];

		wall_found = game->is_wall( cell_to_test );
	}

	if( !wall_found )
		return std::make_pair( -1, glm::vec2() );

	return std::make_pair( walk_side, glm::vec2( { ray_start + ray_dir * current_side_distance } ) );
}

void Player::paint_direction()
{
	constexpr glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 dir = get_trans_matrix() * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );

	game->draw_line( std::pair<glm::vec3, glm::vec3>( position, dir ), red );
}

void Player::paint_camera_projection()
{
	constexpr glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	constexpr glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );

	const glm::vec4 cam_left = get_trans_matrix() * glm::vec4( 1.0F, -zoom, 0.0F, 1.0F );
	const glm::vec4 cam_right = get_trans_matrix() * glm::vec4( 1.0F, zoom, 0.0F, 1.0F );
	const glm::vec4 ray_left = get_trans_matrix() * glm::vec4( 2.0F, -2.0 * zoom, 0.0F, 1.0F );
	const glm::vec4 ray_right = get_trans_matrix() * glm::vec4( 2.0F, 2.0 * zoom, 0.0F, 1.0F );

	game->draw_line( std::pair<glm::vec3, glm::vec3>( cam_left, cam_right ), green );
	game->draw_line( std::pair<glm::vec3, glm::vec3>( position, ray_left ), blue );
	game->draw_line( std::pair<glm::vec3, glm::vec3>( position, ray_right ), blue );
}

void Player::paint_character()
{
	constexpr glm::vec4 yellow = glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	game->draw_point( position, 6.0, yellow );
}