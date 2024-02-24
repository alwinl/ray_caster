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
	// const int total_width = 800;
	// const float local_zoom = zoom;

	// auto calc_coord = [&local_zoom, &total_width](int t){ return (local_zoom * (2 * t / float(total_width) - 1)); };

	// const glm::mat4 trans = get_trans_matrix();

	// const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	// const glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	//  const glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );

	// const glm::vec4 dir = trans * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );

	// const glm::vec4 cam_left = trans * glm::vec4( 1.0F, -1.0F, 0.0F, 1.0F );
	// const glm::vec4 cam_right = trans * glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );
	//  const glm::vec4 left_ray = trans * glm::vec4( 2.0F, 2 * calc_coord(total_width), 0.0F, 1.0F );
	//  const glm::vec4 right_ray = trans * glm::vec4( 2.0F, 2 * calc_coord(0), 0.0F, 1.0F );

	// game->draw_line( position, dir, red );
	// game->draw_line( cam_left, cam_right, green );
	//  game->draw_line( position, left_ray, blue );
	//  game->draw_line( position, right_ray, blue );

	paint_rays( game );
	paint_direction( game );
	paint_camera_projection( game );
	paint_character( game );
}

void Player::paint_rays( TilePaintingGame *game )
{
	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );

	const glm::ivec3 cell( position / scale_factor );
	const glm::ivec3 offset( position - glm::vec3( cell * scale_factor ) );

	const int resolution = 10; // screen_width

	float rot_angle = glm::radians( angle ) + std::atan( zoom );
	const float delta_angle = 2.0F * std::atan( zoom ) / resolution;

	for( int step = 0; step <= resolution; ++step ) {

		const glm::vec3 direction = { cos( rot_angle ), sin( rot_angle ), 0.0 };

		// tan(angle) = opp / adj = sin(angle) / cos(angle) = -dy / dx
		const glm::vec3 delta_distance = { -tan( rot_angle ), 1.0 / tan( rot_angle ), 0.0 };

		// hyp * cos(angle) = x => hyp = x/cos(angle), given x = 1 => hyp = 1/cos(angle)
		// hyp * sin(angle) = y => hyp = y/sin(angle), given y = 1 => hyp = 1/sin(angle)
		// In graphic systems y increases downwards, so invert
		const glm::vec3 travel_distance = { std::abs( cos( rot_angle ) ) < 0.001 ? 1e30 : 1 / cos( rot_angle ),
											std::abs( sin( rot_angle ) ) < 0.001 ? 1e30 : 1 / sin( rot_angle ), 0.0 };

		// game->draw_line( position, position + glm::vec3(1.0, travel_distance[1], 0.0F), blue );
		// game->draw_line( position, position + glm::vec3(travel_distance[0], 1.0, 0.0F), red );

		game->draw_line( position, position + direction * scale_factor, blue );

		// game->draw_line( position, position + direction * glm::vec3(scale_factor, scale_factor * delta_distance[0],
		// 0.0F), red );
		//  game->draw_line( position, position + direction * glm::vec3(scale_factor * delta_distance[0], scale_factor,
		//  red );

		rot_angle -= delta_angle;
	}
}
// #include <iostream>

// void Player::paint_rays( TilePaintingGame * game )
// {
// 	const glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );
// 	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
// 	const int total_width = 800;
// 	const float local_zoom = zoom;

// 	static bool is_printed = false;

// 	const int mapX = static_cast<int>(position[0]/scale_factor);
// 	const int mapY = static_cast<int>(position[1]/scale_factor);

// 	//std::cout << "Position: (" << mapX << ", " << mapY << "), ";

// 	auto calc_coord = [&local_zoom, &total_width](int time){ return (local_zoom * (2 * time / float(total_width) - 1));
// };

// 	const glm::mat4 trans = get_trans_matrix();

// 	//trans = glm::scale( trans, glm::vec3(0.4));

// 	for( int t=401; t <= total_width; t+=400 ) {

// 		const glm::vec2 unit_vector = glm::normalize( glm::vec2( 1.0F, calc_coord(t) ) );
// 		glm::vec4 ray = trans * glm::vec4( unit_vector, 0.0, 1.0 );

// 		std::cout << "Stepping " << ( (ray[0] < position[0]) ? "left" : "right") << ray[0];
// 		std::cout << ", " << ( (ray[1] < position[1]) ? "up" : "down") << ray[1];
// 		std::cout << '\r' << std::ends;

// 		int boundaryX = static_cast<int>(position[0]/scale_factor) * scale_factor;
// 		int boundaryY = static_cast<int>(position[1]/scale_factor) * scale_factor;

// 		// find the cell boundary
// 		if( ray[0] > position[0] ) boundaryX += scale_factor;
// 		if( ray[1] > position[1] ) boundaryY += scale_factor;

// 		// what are we clamping to, ie what is closer
// 		int distanceX = std::abs( boundaryX - position[0] );
// 		int distanceY = std::abs( boundaryY - position[1] );;

// 		if( distanceX > distanceY )
// 			ray[0] = boundaryX;
// 		else
// 			ray[1] = boundaryY;

// 		double coord = std::abs(calc_coord(t));
// 		double coord_inv = 1 / coord;

// 		if( std::abs(coord) < std::abs(coord_inv) ) {
// 			//const glm::vec4 ray = trans * glm::vec4( 1.0F, coord, 0.0F, 1.0F );

// 			//const glm::vec4 ray = trans * glm::vec4(unit_vector *2/** coord*/, 0.0, 1.0 );

// 			game->draw_line( position, ray, blue );
// 		} else {
// 			//const glm::vec4 ray = trans * glm::vec4( coord_inv, 1.0F, 0.0F, 1.0F );
// 			//const glm::vec4 ray = trans * glm::vec4(unit_vector *2/*/ coord*/, 0.0, 1.0 );

// 			game->draw_line( position, ray, red );
// 		}

// 	}

// 	is_printed = true;
// }

void Player::paint_direction( TilePaintingGame *game )
{
	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 dir = get_trans_matrix() * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );

	game->draw_line( position, dir, red );
}

void Player::paint_camera_projection( TilePaintingGame *game )
{
	const glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	const glm::vec4 cam_left = get_trans_matrix() * glm::vec4( 1.0F, -1.0F, 0.0F, 1.0F );
	const glm::vec4 cam_right = get_trans_matrix() * glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	game->draw_line( cam_left, cam_right, green );
}

void Player::paint_character( TilePaintingGame *game )
{
	const glm::vec4 yellow = glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	game->draw_point( position, 6.0, yellow );
}