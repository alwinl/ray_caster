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

#include <iostream>

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <string>
#include <vector>

#include "sdl2wrapper.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SetupParams TilePaintingGame::get_params()
{
	return SetupParams( { "Ray Caster", world_dimension[0] * unit_size * 2, world_dimension[1] * unit_size, 0,
						  SDL_RENDERER_ACCELERATED } );
}

void TilePaintingGame::setup() {}

bool TilePaintingGame::process_event( SDL_Event &event )
{
	if( event.type == SDL_KEYDOWN ) {
		switch( event.key.keysym.sym ) {
		case SDLK_UP: KEY_UP |= KEY_IS_DOWN; break;
		case SDLK_DOWN: KEY_DOWN |= KEY_IS_DOWN; break;
		case SDLK_LEFT: KEY_LEFT |= KEY_IS_DOWN; break;
		case SDLK_RIGHT: KEY_RIGHT |= KEY_IS_DOWN; break;
		case SDLK_x: KEY_X |= KEY_IS_DOWN; break;
		case SDLK_z: KEY_Z |= KEY_IS_DOWN; break;
		}
	}

	if( event.type == SDL_KEYUP ) {
		switch( event.key.keysym.sym ) {
		case SDLK_UP: KEY_UP &= ~KEY_IS_DOWN; break;
		case SDLK_DOWN: KEY_DOWN &= ~KEY_IS_DOWN; break;
		case SDLK_LEFT: KEY_LEFT &= ~KEY_IS_DOWN; break;
		case SDLK_RIGHT: KEY_RIGHT &= ~KEY_IS_DOWN; break;
		case SDLK_x: KEY_X &= ~KEY_IS_DOWN; break;
		case SDLK_z: KEY_Z &= ~KEY_IS_DOWN; break;
		case SDLK_ESCAPE: quit = true; break;
		}
	}

	if( event.type == SDL_QUIT )
		quit = true;

	return quit;
}

void TilePaintingGame::update_state( uint64_t /*elapsed_time*/ )
{
	if( ( KEY_UP & KEY_IS_DOWN ) != 0 )
		hero.move_forward();
	if( ( KEY_DOWN & KEY_IS_DOWN ) != 0 )
		hero.move_back();
	if( ( KEY_LEFT & KEY_IS_DOWN ) != 0 )
		hero.turn_left();
	if( ( KEY_RIGHT & KEY_IS_DOWN ) != 0 )
		hero.turn_right();
	if( ( KEY_X & KEY_IS_DOWN ) != 0 )
		hero.zoom_out();
	if( ( KEY_Z & KEY_IS_DOWN ) != 0 )
		hero.zoom_in();
}

void TilePaintingGame::draw_frame( SDL_Renderer * /*window*/ )
{
	draw_grid();
	draw_level();

	hero.draw( this );
}

void TilePaintingGame::draw_grid()
{
	const glm::vec4 grid_color = { 0.3, 0.3, 0.3, 1.0 };

	for( int line = 0; line <= world_dimension[0]; ++line )
		draw_line( glm::vec3( line * unit_size, 0, 0 ),
				   glm::vec3( line * unit_size, world_dimension[1] * unit_size, 0 ), grid_color );

	for( int line = 0; line <= world_dimension[1]; ++line )
		draw_line( glm::vec3( 0, line * unit_size, 0 ),
				   glm::vec3( world_dimension[0] * unit_size, line * unit_size, 0 ), grid_color );
}

void TilePaintingGame::draw_level()
{
	const glm::vec4 white = { 1.0F, 1.0F, 1.0F, 1.0F };
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> square_corners = make_rect();

	const glm::vec3 z_axis = glm::vec3( 0.0, 0.0, 1.0 );

	glm::mat4 rect_tr( 1.0F );
	rect_tr = glm::translate( rect_tr, glm::vec3( 0.5 ) );
	rect_tr = glm::rotate( rect_tr, glm::radians( 45.0F ), z_axis );
	rect_tr = glm::scale( rect_tr, glm::vec3( 0.707 ) );

	std::transform( square_corners.begin(), square_corners.end(), square_corners.begin(),
					[&rect_tr]( glm::vec4 point ) { return rect_tr * point; } );

	vertices.resize( square_corners.size() );

	for( int cell = 0; cell < world_dimension[0] * world_dimension[1]; ++cell ) {
		if( level[cell] == '1' ) {

			glm::mat4 trans( glm::translate( glm::mat4( static_cast<float>( unit_size ) ),
											 glm::vec3( static_cast<int>( cell % world_dimension[0] ),
														static_cast<int>( cell / world_dimension[0] ), 0.0F ) ) );

			std::transform( square_corners.begin(), square_corners.end(), vertices.begin(),
							[&trans]( glm::vec4 point ) { return trans * point; } );

			draw_geometry( vertices, white );
		}
	}
}

std::vector<glm::vec4> TilePaintingGame::make_primitive( int total_segments, bool filled )
{
	std::vector<glm::vec4> vertex_points;

	const auto delta_angle = static_cast<float>( glm::radians( 360.0 / total_segments ) );
	glm::vec2 outer_point = glm::vec2( 1.0, 0.0 );

	for( int segment = 0; segment < total_segments; ++segment ) {

		if( filled )
			vertex_points.emplace_back( 0.0, 0.0, 0.0, 1.0 );

		vertex_points.emplace_back( outer_point, 0.0, 1.0 );

		outer_point = glm::rotate( outer_point, delta_angle );

		vertex_points.emplace_back( outer_point, 0.0, 1.0 );
	}

	return vertex_points;
}

void TilePaintingGame::draw_line( glm::vec3 point_from, glm::vec3 point_to, glm::vec4 color )
{
	SDL_SetRenderDrawColor( window, static_cast<Uint8>( color[0] * 255 ), static_cast<Uint8>( color[1] * 255 ),
							static_cast<Uint8>( color[2] * 255 ), static_cast<Uint8>( color[3] * 255 ) );
	SDL_RenderDrawLine( window, static_cast<int>( point_from[0] ), static_cast<int>( point_from[1] ),
						static_cast<int>( point_to[0] ), static_cast<int>( point_to[1] ) );
}

void TilePaintingGame::draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 color )
{
	std::vector<SDL_Vertex> vertices;

	vertices.resize( vertex_points.size() );

	std::transform( vertex_points.begin(), vertex_points.end(), vertices.begin(), [&color]( glm::vec4 point ) {
		return SDL_Vertex( { SDL_FPoint{ point[0], point[1] },
							 SDL_Color{ static_cast<Uint8>( color[0] * 255 ), static_cast<Uint8>( color[1] * 255 ),
										static_cast<Uint8>( color[2] * 255 ) },
							 SDL_FPoint{ 0 } } );
	} );

	SDL_RenderGeometry( window, nullptr, vertices.data(), static_cast<int>( vertices.size() ), nullptr, 0 );
}