/*
 * main.cc Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

#include <iostream>

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <string>
#include <vector>

#include "SDL2_gfxPrimitives.h"
#include "sdl2wrapper.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int unit_size = 80;

class TilePaintingGame;

class Player
{
public:
	Player( int init_x, int init_y ) : position( init_x, init_y, 0.0 ){};

	void draw( TilePaintingGame *game );

	void move_forward()
	{
		glm::mat4 trans = get_trans_matrix();
		trans = glm::scale( trans, glm::vec3( 0.05, 0.05, 1 ) );
		position = trans * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	}

	void move_back()
	{
		glm::mat4 trans = get_trans_matrix();
		trans = glm::scale( trans, glm::vec3( 0.05, 0.05, 1 ) );
		position = trans * glm::vec4( -1.0F, 0.0F, 0.0F, 1.0F );
	}

	void turn_left() { angle -= 1.0; }
	void turn_right() { angle += 1.0; }
	void zoom_in() { zoom -= 0.01; }
	void zoom_out() { zoom += 0.01; }

private:
	glm::vec3 position;
	float angle = 0.0;
	float zoom = 1.0;

	glm::mat4 get_trans_matrix()
	{
		glm::mat4 trans = glm::mat4( 1.0F );
		trans = glm::translate( trans, position );
		trans = glm::rotate( trans, glm::radians( angle ), glm::vec3( 0.0F, 0.0F, 1.0F ) );
		trans = glm::scale( trans, glm::vec3( unit_size, unit_size, 1 ) );

		return trans;
	}
};

class TilePaintingGame : public Game
{
private:
	SetupParams get_params() override;
	void setup() override;
	bool process_event( SDL_Event &event ) override;
	void update_state( uint64_t elapsed_time ) override;
	void draw_frame( SDL_Renderer *window ) override;

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

	Player hero{ 100, 100 };

	bool quit = false;

	uint8_t KEY_UP = 0;
	uint8_t KEY_DOWN = 0;
	uint8_t KEY_LEFT = 0;
	uint8_t KEY_RIGHT = 0;
	uint8_t KEY_X = 0;
	uint8_t KEY_Z = 0;

	uint8_t KEY_IS_DOWN = 0x01;

	SDL_Renderer *window = nullptr;

public:
	void draw_line( glm::vec3 point_from, glm::vec3 point_to, glm::vec4 color );
	void draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 color );
};

void Player::draw( TilePaintingGame *game )
{
	const glm::mat4 trans = get_trans_matrix();
	const glm::vec4 red = glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 green = glm::vec4( 0.0F, 1.0F, 0.0F, 1.0F );
	const glm::vec4 blue = glm::vec4( 0.0F, 0.0F, 1.0F, 1.0F );
	const glm::vec4 yellow = glm::vec4( 1.0F, 1.0F, 0.0F, 1.0F );

	std::vector<glm::vec4> player_geometry = {
		glm::vec4( 0.1F, 0.0F, 0.0F, 1.0F ),
		glm::vec4( -0.1F, -0.1F, 0.0F, 1.0F ),
		glm::vec4( -0.1F, 0.1F, 0.0F, 1.0F ),
	};

	const glm::vec4 dir = trans * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	const glm::vec4 cam_left = trans * glm::vec4( 1.0F, zoom, 0.0F, 1.0F );
	const glm::vec4 cam_right = trans * glm::vec4( 1.0F, -zoom, 0.0F, 1.0F );
	const glm::vec4 left_ray = trans * glm::vec4( 2.0F, 2 * zoom, 0.0F, 1.0F );
	const glm::vec4 right_ray = trans * glm::vec4( 2.0F, 2 * -zoom, 0.0F, 1.0F );

	std::transform( player_geometry.begin(), player_geometry.end(), player_geometry.begin(),
					[&trans]( glm::vec4 point ) { return trans * point; } );

	game->draw_line( position, dir, red );
	game->draw_line( cam_left, cam_right, green );
	game->draw_line( position, left_ray, blue );
	game->draw_line( position, right_ray, blue );
	game->draw_geometry( player_geometry, yellow );
}

SetupParams TilePaintingGame::get_params()
{
	return SetupParams( { "Ray Caster", 10 * unit_size * 2, 10 * unit_size, 0, SDL_RENDERER_ACCELERATED } );
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

void TilePaintingGame::draw_frame( SDL_Renderer *window )
{
	this->window = window;

	draw_grid();
	draw_level();

	hero.draw( this );
}

void TilePaintingGame::draw_grid()
{
	const glm::vec4 grid_color = { 0.3, 0.3, 0.3, 1.0 };

	for( int line = 0; line <= 10; ++line ) {
		draw_line( glm::vec3( line * unit_size, 0, 0 ), glm::vec3( line * unit_size, 10 * unit_size, 0 ), grid_color );
		draw_line( glm::vec3( 0, line * unit_size, 0 ), glm::vec3( 10 * unit_size, line * unit_size, 0 ), grid_color );
	}
}

void TilePaintingGame::draw_level()
{
	const glm::vec4 white = { 1.0F, 1.0F, 1.0F, 1.0F };

	const auto funit_size = static_cast<float>( unit_size );

	for( int cell = 0; cell < 100; ++cell ) {
		if( level[cell] == '1' ) {
			const float top_x = static_cast<float>( cell % 10 ) * funit_size;
			const float top_y = static_cast<float>( cell / 10.0 ) * funit_size;
			const float bottom_x = top_x + funit_size;
			const float bottom_y = top_y + funit_size;

			std::vector<glm::vec4> square_corners = {
				glm::vec4( top_x, top_y, 0.0F, 1.0F ), // top right triangle
				glm::vec4( bottom_x, top_y, 0.0F, 1.0F ),	 glm::vec4( bottom_x, bottom_y, 0.0F, 1.0F ),

				glm::vec4( top_x, top_y, 0.0F, 1.0F ), // bottom left triangle
				glm::vec4( bottom_x, bottom_y, 0.0F, 1.0F ), glm::vec4( top_x, bottom_y, 0.0F, 1.0F ),
			};

			draw_geometry( square_corners, white );
		}
	}
}

void TilePaintingGame::draw_line( glm::vec3 point_from, glm::vec3 point_to, glm::vec4 color )
{
	/* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
	SDL_SetRenderDrawColor( window, static_cast<Uint8>( color.r * 255 ), static_cast<Uint8>( color.g * 255 ),
							/* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
							static_cast<Uint8>( color.b * 255 ), static_cast<Uint8>( color.a * 255 ) );
	/* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
	SDL_RenderDrawLine( window, static_cast<int>( point_from.x ), static_cast<int>( point_from.y ),
						/* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
						static_cast<int>( point_to.x ), static_cast<int>( point_to.y ) );
}

void TilePaintingGame::draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 color )
{
	std::vector<SDL_Vertex> vertices;

	vertices.resize( vertex_points.size() );

	std::transform( vertex_points.begin(), vertex_points.end(), vertices.begin(), [&color]( glm::vec4 point ) {
		/* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
		return SDL_Vertex( { SDL_FPoint{ point.x, point.y },
							 /* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
							 SDL_Color{ static_cast<Uint8>( color.r * 255 ), static_cast<Uint8>( color.g * 255 ),
										/* trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access) */
										static_cast<Uint8>( color.b * 255 ) },
							 SDL_FPoint{ 0 } } );
	} );

	SDL_RenderGeometry( window, nullptr, vertices.data(), static_cast<int>( vertices.size() ), nullptr, 0 );
}

int main( int /*unused*/, char ** /*unused*/ )
{
	GameWrapper<TilePaintingGame> the_game;
	return the_game.run();
}