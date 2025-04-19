/*
 * sdl2wrapper.h Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct SetupParams
{
	std::string title;
	int width;
	int height;
	uint32_t flags; // potential to make this an enum class
	int rendererFlags = 0;
};

class SDL_Wrapper
{
public:
	SDL_Wrapper() { SDL_Init( SDL_INIT_VIDEO ); }
	~SDL_Wrapper()
	{
		SDL_DestroyRenderer( renderer );
		SDL_DestroyWindow( window );
		SDL_Quit();
	}
	SDL_Wrapper( const SDL_Wrapper& other ) = delete;
	SDL_Wrapper( SDL_Wrapper&& other ) = delete;
	SDL_Wrapper& operator=( const SDL_Wrapper& other ) = delete;
	SDL_Wrapper& operator=( SDL_Wrapper&& other ) = delete;

	void create_window( SetupParams params )
	{
		this->params = params;

		if( ! SDL_CreateWindowAndRenderer( params.title.c_str(), params.width, params.height, params.flags, &window, &renderer ) != 0 )
			throw std::runtime_error( SDL_GetError() );
	}

	void draw_background( glm::u8vec3 color = {0,0,0} ) {
		SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, 255 );
		SDL_RenderClear( renderer );
	}

	void display_window() {
		SDL_RenderPresent( renderer );
	}

	void draw_point( glm::vec3 center, float radius, const glm::vec4 colour )
	{
		constexpr int total_segments = 32;

		glm::mat4x4 trans( 1.0F );
		trans = glm::translate( trans, center );
		trans = glm::scale( trans, glm::vec3( radius ) );

		const auto delta_angle = glm::radians( 360.0F / (1.0F * total_segments) );
		glm::vec2 outer_point = glm::vec2( 1.0, 0.0 );
		std::vector<glm::vec4> circle_verts;

		for( int segment = 0; segment < total_segments; ++segment ) {

			circle_verts.emplace_back( trans * glm::vec4(0.0, 0.0, 0.0, 1.0) );
			circle_verts.emplace_back( trans * glm::vec4(outer_point, 0.0, 1.0) );

			outer_point = glm::rotate( outer_point, delta_angle );

			circle_verts.emplace_back( trans * glm::vec4(outer_point, 0.0, 1.0) );
		}

		draw_geometry( circle_verts, colour );
	}

	void draw_line( std::pair<glm::vec3, glm::vec3> points, glm::vec4 colour )
	{
		glm::u8vec4 temp( colour * 255 );
		glm::ivec3 pt_from( points.first );
		glm::ivec3 pt_to( points.second );

		pt_from.x = std::clamp( pt_from.x, 0, params.width  );
		pt_from.y = std::clamp( pt_from.y, 0, params.height );
		pt_to.x   = std::clamp( pt_to.x,   0, params.width  );
		pt_to.y   = std::clamp( pt_to.y,   0, params.height );

		SDL_SetRenderDrawColor( renderer, temp.r, temp.g, temp.b, temp.a );
		SDL_RenderLine( renderer, pt_from.x, pt_from.y, pt_to.x, pt_to.y );
	}

	void draw_rect( std::pair<glm::vec4,glm::vec4> points, glm::vec4 colour )
	{
		glm::ivec4 pt_lt( points.first );		// left top
		glm::ivec4 pt_rb( points.second );		// bottom right

		pt_lt.x = std::clamp( pt_lt.x, 0, params.width  );
		pt_lt.y = std::clamp( pt_lt.y, 0, params.height );
		pt_rb.x = std::clamp( pt_rb.x, 0, params.width  );
		pt_rb.y = std::clamp( pt_rb.y, 0, params.height );

		std::vector<glm::vec4> verts;
		verts.emplace_back( pt_lt.x, pt_lt.y, 0.0, 1.0 );
		verts.emplace_back( pt_rb.x, pt_rb.y, 0.0, 1.0 );
		verts.emplace_back( pt_lt.x, pt_rb.y, 0.0, 1.0 );

		verts.emplace_back( pt_lt.x, pt_lt.y, 0.0, 1.0 );
		verts.emplace_back( pt_rb.x, pt_lt.y, 0.0, 1.0 );
		verts.emplace_back( pt_rb.x, pt_rb.y, 0.0, 1.0 );

		draw_geometry( verts, colour );
	}

	void draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 color )
	{
		glm::vec3 temp = color * 255;
		SDL_FColor colour = { temp.r, temp.g, temp.b };
		SDL_FPoint texture_uv = { 0, 0 };

		std::vector<SDL_Vertex> vertices;
		vertices.resize( vertex_points.size() );

		std::transform( vertex_points.begin(), vertex_points.end(), vertices.begin(), [&]( glm::vec4 point ) {
			const SDL_FPoint vert( std::clamp( point.x, 0.0F, params.width * 1.0F ),
								std::clamp( point.y, 0.0F, params.height * 10.F) );
			return SDL_Vertex( vert, colour, texture_uv );

		} );

		SDL_RenderGeometry( renderer, nullptr, vertices.data(), static_cast<int>( vertices.size() ), nullptr, 0 );
	}

	uint64_t get_ticks() { return SDL_GetTicks(); }

private:
	SDL_Renderer *renderer = nullptr;
	SDL_Window *window = nullptr;
	SetupParams params;
};

class Game
{
public:
	Game() = default;
	virtual ~Game() = default;

	SetupParams make_setup() { return get_params(); }
	void initialise( SDL_Wrapper *sdl_wrapper )
	{
		this->sdl_wrapper = sdl_wrapper;
		setup();
	}
	bool input( SDL_Event &event ) { return process_event( event ); };
	void update( uint64_t elapsed_time ) { update_state( elapsed_time ); }
	void draw() { draw_frame(); }

protected:
	virtual SetupParams get_params() = 0;
	virtual void setup() = 0;
	virtual bool process_event( SDL_Event &event ) = 0;
	virtual void update_state( uint64_t elapsed_time ) = 0;
	virtual void draw_frame() = 0;

	void draw_point( glm::vec3 center, float radius, const glm::vec4 colour ) { sdl_wrapper->draw_point( center, radius, colour);}
	void draw_line( std::pair<glm::vec3, glm::vec3> points, glm::vec4 colour ) { sdl_wrapper->draw_line( points, colour);}
	void draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 colour ) { sdl_wrapper->draw_geometry( vertex_points, colour);}
	void draw_rect( std::pair<glm::vec4,glm::vec4> points, glm::vec4 colour ) {sdl_wrapper->draw_rect(points, colour);}
	void draw_background( glm::u8vec3 colour ) {sdl_wrapper->draw_background(colour);}

	SDL_Wrapper *sdl_wrapper = nullptr;
};

template <typename T> class GameWrapper
{
public:
	GameWrapper() = default;
	~GameWrapper() = default;

	GameWrapper( const GameWrapper& other ) = delete;
	GameWrapper& operator=( const GameWrapper& other ) = delete;
	GameWrapper( GameWrapper&& other ) = delete;
	GameWrapper& operator=( GameWrapper& other ) = delete;

	int run()
	{
		SDL_Wrapper win;
		T aGame;

		SetupParams params = aGame.make_setup();
		win.create_window( params );

		aGame.initialise( &win );

		uint64_t frame_tick = win.get_ticks();

		bool quit = false;

		while( !quit ) {

			SDL_Event event;

			while( SDL_PollEvent( &event ) )
				quit = aGame.input( event );

			if( quit )
				break;

			if( win.get_ticks() > ( frame_tick + 16 ) ) {

				aGame.update( win.get_ticks() - frame_tick );

				win.draw_background();

				aGame.draw();

				win.display_window();

				frame_tick = win.get_ticks();
			}
		}

		return 0;
	};

private:
};

class BlankGame : public Game
{
	SetupParams get_params() override { return SetupParams( { "Blank Game", 640, 480, 0, 0 } ); }
	void setup() override {}
	bool process_event( SDL_Event &event ) override { return event.type == SDL_EVENT_QUIT; }
	void update_state( uint64_t elapsed_time ) override{};
	void draw_frame() override{};
};
