/*
 * tile_painting_game.cc Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

#include "tile_painting_game.h"

// #include <iostream>

// #include <algorithm>
// #include <array>
// #include <cmath>
// #include <iterator>
// #include <string>
// #include <vector>

#include "sdl2wrapper.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main( int /*unused*/, char ** /*unused*/ )
{
	GameWrapper<TilePaintingGame> the_game;
	return the_game.run();
}

SetupParams TilePaintingGame::get_params()
{
	unit_size = 10.0F;

	return SetupParams( { "Ray Caster", screen_width, screen_height, 0,
						  SDL_RENDERER_ACCELERATED } );
}

void TilePaintingGame::setup() {}

bool TilePaintingGame::process_event( SDL_Event &event )
{
	if( event.type == SDL_KEYDOWN ) {
		switch( event.key.keysym.sym ) {
		case SDLK_UP:    key_state |= 1 << KEY_UP;    break;
		case SDLK_DOWN:  key_state |= 1 << KEY_DOWN;  break;
		case SDLK_LEFT:  key_state |= 1 << KEY_LEFT;  break;
		case SDLK_RIGHT: key_state |= 1 << KEY_RIGHT; break;
		case SDLK_x:     key_state |= 1 << KEY_X;     break;
		case SDLK_z:     key_state |= 1 << KEY_Z;     break;
		}
	}

	if( event.type == SDL_KEYUP ) {
		switch( event.key.keysym.sym ) {
		case SDLK_UP:    key_state &= ~(1 << KEY_UP);    break;
		case SDLK_DOWN:  key_state &= ~(1 << KEY_DOWN);  break;
		case SDLK_LEFT:  key_state &= ~(1 << KEY_LEFT);  break;
		case SDLK_RIGHT: key_state &= ~(1 << KEY_RIGHT); break;
		case SDLK_x:     key_state &= ~(1 << KEY_X);     break;
		case SDLK_z:     key_state &= ~(1 << KEY_Z);     break;
		case SDLK_ESCAPE: quit = true;                   break;
		}
	}

	if( event.type == SDL_QUIT )
		quit = true;

	return quit;
}

void TilePaintingGame::update_state( uint64_t elapsed_time )
{
	if( key_state & (1 << KEY_UP) ) {
		const glm::vec3 new_position = player_matrix * glm::vec4( 0.005F * elapsed_time, 0.0F, 0.0F, 1.0F );

		if( !is_wall( glm::ivec2( new_position[0] / unit_size,	new_position[1] / unit_size ) ) )
			player_position = new_position;
	}

	if( key_state & (1 << KEY_DOWN) ) {
		const glm::vec3 new_position = player_matrix * glm::vec4( -0.005F * elapsed_time, 0.0F, 0.0F, 1.0F );

		if( !is_wall( glm::ivec2( new_position[0] / unit_size,	new_position[1] / unit_size ) ) )
			player_position = new_position;
	}

	if( key_state & (1 << KEY_LEFT) )
		player_angle -= glm::radians( 0.1 * elapsed_time );
	if( key_state & (1 << KEY_RIGHT) )
		player_angle += glm::radians( 0.1 * elapsed_time );

	if( key_state & (1 << KEY_X) )
		player_zoom -= 0.001 * elapsed_time;
	if( key_state & (1 << KEY_Z) )
		player_zoom += 0.001 * elapsed_time;

	player_matrix =  glm::mat4( {
		{ unit_size * std::cos( player_angle ), unit_size * std::sin( player_angle ), 0.0F, 0.0F },
		{ -unit_size * std::sin( player_angle ), unit_size * std::cos( player_angle ), 0.0F, 0.0F },
		{ 0.0F, 0.0F, 1.0F, 0.0F },
		{ player_position[0], player_position[1], 0.0F, 1.0F },
	} );
}

void TilePaintingGame::draw_frame()
{
	paint_floor();
	paint_ceiling();
	paint_rays();

	// minimap
	paint_grid();
	paint_level();
	paint_direction();
	paint_camera_projection();
	paint_character();
}

void TilePaintingGame::paint_floor()
{
	constexpr glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	
	std::pair<glm::vec4,glm::vec4> points{ 
		glm::vec4(0.0F, screen_height/2.0F, 0.0F, 1.0F), 
		glm::vec4(screen_width * 1.0F, screen_height * 1.0F, 0.0F, 1.0F)
	};

	draw_rect( points, green ); 
}

void TilePaintingGame::paint_ceiling()
{
	constexpr glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 0.8F, 1.0F );

	std::pair<glm::vec4,glm::vec4> points{ 
		glm::vec4(0.0F, 0.0F, 0.0F, 1.0F), 
		glm::vec4(screen_width * 1.0F, screen_height/2.0F, 0.0F, 1.0F)
	};

	draw_rect( points, blue ); 
}

void TilePaintingGame::paint_rays()
{
	constexpr int x_dim = 0;
	constexpr int y_dim = 1;

	constexpr glm::vec4 dark_grey = glm::vec4( 0.75F, 0.75F, 0.75F, 1.0F );
	constexpr glm::vec4 light_grey = glm::vec4( 0.5F, 0.5F, 0.5F, 1.0F );
	constexpr glm::vec4 black = glm::vec4( 0.0F, 0.0F, 0.0F, 1.0F );

	const int resolution = screen_width;

	float rot_angle = player_angle - std::atan( player_zoom );
	const float delta_angle = 2.0F * std::atan( player_zoom ) / ( 1.0F * resolution );

	for( int step = 0; step <= resolution; ++step ) {

		const glm::vec2 ray_start = glm::vec2( player_position ) / unit_size;

		const std::pair<int, glm::vec2> result = calc_intersection( rot_angle, ray_start );
		const glm::vec2 intersection( result.second * unit_size );
		const int wall_side = result.first;

		if( wall_side != -1 ) {

			glm::vec2 delta = intersection - glm::vec2(player_position);

			// credit: https://www.youtube.com/watch?v=eOCQfxRQ2pY
			const float distance = delta[x_dim] * std::cos( player_angle ) + delta[y_dim] * std::sin( player_angle );

			const float height = unit_size * 400.0F / distance;
			const std::pair<glm::vec3, glm::vec3> points = {
				glm::vec3( step, (screen_height / 2.0) - height, 0 ),
				glm::vec3( step, (screen_height / 2.0) + height, 0 ) };

			const float horz_offset = (1.0F * intersection[x_dim]) / unit_size;

			if( horz_offset < .0 )
				draw_line( points, black );
			else
				draw_line( points, ( wall_side == x_dim ) ? light_grey : dark_grey );
		}

		rot_angle += delta_angle;
	}
}

void TilePaintingGame::paint_grid()
{
	constexpr glm::vec4 grid_color = { 0.3, 0.3, 0.3, 1.0 };

	for( int line = 0; line <= world_dimension[0]; ++line )
		draw_line( { {line * unit_size, 0, 0}, {line * unit_size, world_dimension[1] * unit_size, 0}  }, grid_color );

	for( int line = 0; line <= world_dimension[1]; ++line )
		draw_line( { { 0, line * unit_size, 0},	{world_dimension[0] * unit_size, line * unit_size, 0} }, grid_color );
}

void TilePaintingGame::paint_level()
{
	constexpr glm::vec4 white = { 1.0F, 1.0F, 1.0F, 1.0F };
	constexpr glm::vec4 black = { 0.0F, 0.0F, 0.0F, 1.0F };

	std::pair<glm::vec4,glm::vec4> rect_points;

	for( int cell = 0; cell < world_dimension[0] * world_dimension[1]; ++cell ) {
			rect_points.first = glm::vec4{ (cell % world_dimension[0]) * unit_size, (cell / world_dimension[0]) * unit_size,0.0F, 1.0F };
			rect_points.second = rect_points.first + glm::vec4( unit_size, unit_size, 0.0F, 1.0F );
			
			draw_rect( rect_points, ( level[cell] == '1' ) ? white : black );
	}
}

void TilePaintingGame::paint_direction()
{
	constexpr glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 dir = player_matrix * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );

	draw_line( std::pair<glm::vec3, glm::vec3>( player_position, dir ), red );
}

void TilePaintingGame::paint_camera_projection()
{
	constexpr glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	constexpr glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );

	const glm::vec4 cam_left = player_matrix * glm::vec4( 1.0F, -player_zoom, 0.0F, 1.0F );
	const glm::vec4 cam_right = player_matrix * glm::vec4( 1.0F, player_zoom, 0.0F, 1.0F );
	const glm::vec4 ray_left = player_matrix * glm::vec4( 2.0F, -2.0 * player_zoom, 0.0F, 1.0F );
	const glm::vec4 ray_right = player_matrix * glm::vec4( 2.0F, 2.0 * player_zoom, 0.0F, 1.0F );

	draw_line( std::pair<glm::vec3, glm::vec3>( cam_left, cam_right ), green );
	draw_line( std::pair<glm::vec3, glm::vec3>( player_position, ray_left ), blue );
	draw_line( std::pair<glm::vec3, glm::vec3>( player_position, ray_right ), blue );
}

void TilePaintingGame::paint_character()
{
	constexpr glm::vec4 yellow = glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	draw_point( player_position, 6.0, yellow );
}

// adapted based on https://www.youtube.com/watch?v=NbSee-XM7WA
std::pair<int, glm::vec2> TilePaintingGame::calc_intersection( float angle, glm::vec2 ray_start )
{
	constexpr int x_dim = 0;
	constexpr int y_dim = 1;

	const glm::vec2 ray_dir = { std::cos( angle ), std::sin( angle ) };

	const glm::ivec2 step = {
		(ray_dir[x_dim] < 0) ? -1 : 1, 
		(ray_dir[y_dim] < 0) ? -1 : 1
	};

	const glm::vec2 unit_step_size = {
		std::sqrt( 1 + ( ray_dir[y_dim] / ray_dir[x_dim] ) * ( ray_dir[y_dim] / ray_dir[x_dim] ) ),
		std::sqrt( 1 + ( ray_dir[x_dim] / ray_dir[y_dim] ) * ( ray_dir[x_dim] / ray_dir[y_dim] ) )
	};

	glm::ivec2 cell_to_test = ray_start;
	const glm::vec2 first_offset = ( ray_start - glm::vec2(cell_to_test) );
	glm::vec2 ray_length_by_dimension = {
		( ( ray_dir[x_dim] < 0 ) ? first_offset[x_dim] : ( 1.0F - first_offset[x_dim] ) ) * unit_step_size[x_dim],
		( ( ray_dir[y_dim] < 0 ) ? first_offset[y_dim] : ( 1.0F - first_offset[y_dim] ) ) * unit_step_size[y_dim]
	};

	constexpr float max_distance = 100.0;
	float current_side_distance = 0.0;
	bool wall_found = false;
	int walk_side = -1;

	while( !wall_found && current_side_distance < max_distance ) {

		walk_side = ( ray_length_by_dimension[x_dim] < ray_length_by_dimension[y_dim] ) ? x_dim : y_dim;

		cell_to_test[walk_side] += step[walk_side];
		current_side_distance = ray_length_by_dimension[walk_side];
		ray_length_by_dimension[walk_side] += unit_step_size[walk_side];

		wall_found = is_wall( cell_to_test );
	}

	if( !wall_found )
		return std::make_pair( -1, glm::vec2() );

	return std::make_pair( walk_side, glm::vec2( { ray_start + ray_dir * current_side_distance } ) );
}

bool TilePaintingGame::is_wall( glm::ivec2 cell )
{
	constexpr int x_dim = 0;
	constexpr int y_dim = 1;

	if( cell[x_dim] < 0 || cell[x_dim] > world_dimension[x_dim] 
	 || cell[y_dim] < 0 || cell[y_dim] > world_dimension[y_dim] )
		return false;

	return level[cell[x_dim] + cell[y_dim] * world_dimension[x_dim]] == '1';
}

// std::vector<glm::vec4> TilePaintingGame::make_primitive( int total_segments, bool filled )
// {
// 	std::vector<glm::vec4> vertex_points;

// 	const auto delta_angle = static_cast<float>( glm::radians( 360.0 / total_segments ) );
// 	glm::vec2 outer_point = glm::vec2( 1.0, 0.0 );

// 	for( int segment = 0; segment < total_segments; ++segment ) {

// 		if( filled )
// 			vertex_points.emplace_back( 0.0, 0.0, 0.0, 1.0 );

// 		vertex_points.emplace_back( outer_point, 0.0, 1.0 );

// 		outer_point = glm::rotate( outer_point, delta_angle );

// 		vertex_points.emplace_back( outer_point, 0.0, 1.0 );
// 	}

// 	return vertex_points;
// }
