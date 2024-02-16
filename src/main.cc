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

#include <string>

#include "SDL2_gfxPrimitives.h"
#include "sdl2wrapper.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int unit_size = 80;

class Player
{
public:
	Player( int init_x, int init_y ) : position( init_x, init_y, 0.0 ){};

	void draw( SDL_Renderer *window );

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

	// glm::vec4 transform_point( glm::vec4 point ) { return get_trans_matrix() * point; }
};

void Player::draw( SDL_Renderer *window )
{
	const glm::mat4 trans = get_trans_matrix();

	SDL_SetRenderDrawColor( window, 1 * 255, 1 * 255, 1 * 255, 1 * 255 );

	filledCircleRGBA( window, position.x, position.y, unit_size / 5, 255, 255, 0, 255 );

	SDL_SetRenderDrawColor( window, 1 * 255, 0 * 255, 0 * 255, 1 * 255 );
	const glm::vec4 world_dir = trans * glm::vec4( 1.0F, 0.0F, 0.0F, 1.0F );
	SDL_RenderDrawLine( window, position.x, position.y, world_dir.x, world_dir.y );

	SDL_SetRenderDrawColor( window, 0 * 255, 1 * 255, 0 * 255, 1 * 255 );
	const glm::vec4 world_cam_left = trans * glm::vec4( 1.0F, zoom, 0.0F, 1.0F );
	const glm::vec4 world_cam_right = trans * glm::vec4( 1.0F, -zoom, 0.0F, 1.0F );
	SDL_RenderDrawLine( window, world_cam_left.x, world_cam_left.y, world_cam_right.x, world_cam_right.y );

	SDL_SetRenderDrawColor( window, 0 * 255, 0 * 255, 1 * 255, 1 * 255 );
	const glm::vec4 world_left_ray = trans * glm::vec4( 2.0F, 2 * zoom, 0.0F, 1.0F );
	const glm::vec4 world_right_ray = trans * glm::vec4( 2.0F, 2 * -zoom, 0.0F, 1.0F );
	SDL_RenderDrawLine( window, position.x, position.y, world_left_ray.x, world_left_ray.y );
	SDL_RenderDrawLine( window, position.x, position.y, world_right_ray.x, world_right_ray.y );
}

class TilePaintingGame : public Game
{
public:
	TilePaintingGame() : hero( 100, 100 ){};

private:
	SetupParams get_params() override;
	void setup() override;
	bool process_event( SDL_Event &event ) override;
	void update_state( uint64_t elapsed_time ) override;
	void draw_frame( SDL_Renderer *window ) override;

	void draw_grid( SDL_Renderer *window );
	void draw_level( SDL_Renderer *window );

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

	Player hero;

	bool quit = false;

	uint8_t KEY_UP = 0;
	uint8_t KEY_DOWN = 0;
	uint8_t KEY_LEFT = 0;
	uint8_t KEY_RIGHT = 0;
	uint8_t KEY_X = 0;
	uint8_t KEY_Z = 0;

	uint8_t KEY_IS_DOWN = 0x01;
};

SetupParams TilePaintingGame::get_params()
{
	return SetupParams( { "Blank Game", 10 * unit_size * 2, 10 * unit_size, 0, SDL_RENDERER_ACCELERATED } );
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
	draw_grid( window );
	draw_level( window );

	hero.draw( window );
}

void TilePaintingGame::draw_grid( SDL_Renderer *window )
{
	SDL_SetRenderDrawColor( window, 0.3 * 255, 0.3 * 255, 0.3 * 255, 1 * 255 );

	for( int line = 0; line <= 10; ++line ) {
		SDL_RenderDrawLine( window, line * unit_size, 0, line * unit_size, 10 * unit_size );
		SDL_RenderDrawLine( window, 0, line * unit_size, 10 * unit_size, line * unit_size );
	}
}

void TilePaintingGame::draw_level( SDL_Renderer *window )
{
	SDL_SetRenderDrawColor( window, 1 * 255, 1 * 255, 1 * 255, 1 * 255 );

	for( int cell = 0; cell < 100; ++cell ) {
		if( level[cell] == '1' ) {
			SDL_Rect const rect( { ( cell % 10 ) * unit_size, ( cell / 10 ) * unit_size, unit_size, unit_size } );
			SDL_RenderFillRect( window, &rect );
		}
	}
}

int main( int /*unused*/, char ** /*unused*/ )
{
	GameWrapper<TilePaintingGame> the_game;
	return the_game.run();
}