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

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct SetupParams {
	std::string title;
	int width;
	int height;
	uint32_t flags; // potential to make this an enum class
	int rendererFlags = SDL_RENDERER_ACCELERATED;
};

class SDL_Wrapper
{
public:
	SDL_Wrapper() { SDL_Init( SDL_INIT_EVERYTHING ); }
	~SDL_Wrapper()
	{
		SDL_DestroyRenderer( renderer );
		SDL_DestroyWindow( window );
		SDL_Quit();
	}
	SDL_Wrapper( const SDL_Wrapper &other ) = delete;
	SDL_Wrapper( SDL_Wrapper &&other ) = delete;
	SDL_Wrapper &operator=( const SDL_Wrapper &other ) = delete;
	SDL_Wrapper &operator=( SDL_Wrapper &&other ) = delete;

	void create_window( SetupParams params )
	{
		this->params = params;

		window = SDL_CreateWindow( params.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, params.width,
								   params.height, params.flags );
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );
		renderer = SDL_CreateRenderer( window, -1, params.rendererFlags );
	}

	void clear_window()
	{

		glClearColor( 0.0F, 0.0F, 0.0F, 0.0F );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		// SDL_RenderClear( renderer );
	}

	void display_window() { SDL_RenderPresent( renderer ); }

	void draw_point( glm::vec3 center, float radius, const glm::vec4 colour )
	{
		constexpr int total_segments = 32;

		glm::mat4x4 trans( 1.0F );
		trans = glm::translate( trans, center );
		trans = glm::scale( trans, glm::vec3( radius ) );

		const auto delta_angle = static_cast<float>( glm::radians( 360.0 / total_segments ) );
		glm::vec2 outer_point = glm::vec2( 1.0, 0.0 );
		std::vector<glm::vec4> circle_verts;

		for( int segment = 0; segment < total_segments; ++segment ) {

			circle_verts.emplace_back( trans * glm::vec4( 0.0, 0.0, 0.0, 1.0 ) );
			circle_verts.emplace_back( trans * glm::vec4( outer_point, 0.0, 1.0 ) );

			outer_point = glm::rotate( outer_point, delta_angle );

			circle_verts.emplace_back( trans * glm::vec4( outer_point, 0.0, 1.0 ) );
		}

		draw_geometry( circle_verts, colour );
	}

	void draw_line( std::pair<glm::vec3, glm::vec3> points, glm::vec4 colour )
	{
		glm::u8vec4 temp( colour * 255 );
		glm::ivec3 pt_from( points.first );
		glm::ivec3 pt_to( points.second );

		pt_from[0] = std::clamp( pt_from[0], 0, params.width );
		pt_from[1] = std::clamp( pt_from[1], 0, params.height );
		pt_to[0] = std::clamp( pt_to[0], 0, params.width );
		pt_to[1] = std::clamp( pt_to[1], 0, params.height );

		SDL_SetRenderDrawColor( renderer, temp[0], temp[1], temp[2], temp[3] );
		SDL_RenderDrawLine( renderer, pt_from[0], pt_from[1], pt_to[0], pt_to[1] );
	}

	void draw_rect( std::pair<glm::vec4, glm::vec4> points, glm::vec4 colour )
	{
		constexpr int x_coord = 0;
		constexpr int y_coord = 1;

		glm::ivec4 pt_lt( points.first );  // left top
		glm::ivec4 pt_rb( points.second ); // bottom right

		pt_lt[x_coord] = std::clamp( pt_lt[x_coord], 0, params.width );
		pt_lt[y_coord] = std::clamp( pt_lt[y_coord], 0, params.height );
		pt_rb[x_coord] = std::clamp( pt_rb[x_coord], 0, params.width );
		pt_rb[y_coord] = std::clamp( pt_rb[y_coord], 0, params.height );

		std::vector<glm::vec4> verts;
		verts.emplace_back( pt_lt[x_coord], pt_lt[y_coord], 0.0, 1.0 );
		verts.emplace_back( pt_rb[x_coord], pt_rb[y_coord], 0.0, 1.0 );
		verts.emplace_back( pt_lt[x_coord], pt_rb[y_coord], 0.0, 1.0 );

		verts.emplace_back( pt_lt[x_coord], pt_lt[y_coord], 0.0, 1.0 );
		verts.emplace_back( pt_rb[x_coord], pt_lt[y_coord], 0.0, 1.0 );
		verts.emplace_back( pt_rb[x_coord], pt_rb[y_coord], 0.0, 1.0 );

		draw_geometry( verts, colour );
	}

	void draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 color )
	{
		glm::u8vec3 temp = color * 255;
		SDL_Color colour = { temp[0], temp[1], temp[2] };
		SDL_FPoint texture_uv = { 0, 0 };

		std::vector<SDL_Vertex> vertices;
		vertices.resize( vertex_points.size() );

		std::transform( vertex_points.begin(), vertex_points.end(), vertices.begin(), [&]( glm::vec4 point ) {
			const glm::ivec2 vert( point[0], point[1] );
			const glm::vec2 fvert( std::clamp( vert[0], 0, params.width ), std::clamp( vert[1], 0, params.height ) );
			return SDL_Vertex( { { fvert[0], fvert[1] }, colour, texture_uv } );
		} );

		SDL_RenderGeometry( renderer, nullptr, vertices.data(), static_cast<int>( vertices.size() ), nullptr, 0 );
	}

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

	void draw_point( glm::vec3 center, float radius, const glm::vec4 colour )
	{
		sdl_wrapper->draw_point( center, radius, colour );
	}
	void draw_line( std::pair<glm::vec3, glm::vec3> points, glm::vec4 colour )
	{
		sdl_wrapper->draw_line( points, colour );
	}
	void draw_geometry( std::vector<glm::vec4> &vertex_points, glm::vec4 colour )
	{
		sdl_wrapper->draw_geometry( vertex_points, colour );
	}
	void draw_rect( std::pair<glm::vec4, glm::vec4> points, glm::vec4 colour )
	{
		sdl_wrapper->draw_rect( points, colour );
	}

	SDL_Wrapper *sdl_wrapper = nullptr;
};

template <typename T> class GameWrapper
{
public:
	GameWrapper() = default;
	~GameWrapper() = default;

	GameWrapper( const GameWrapper &other ) = delete;
	GameWrapper &operator=( const GameWrapper &other ) = delete;
	GameWrapper( GameWrapper &&other ) = delete;
	GameWrapper &operator=( GameWrapper &other ) = delete;

	int run()
	{

		params = aGame.make_setup();
		sdl_wrapper.create_window( params );

		aGame.initialise( &sdl_wrapper );

		uint64_t game_tick = SDL_GetTicks64();

		bool quit = false;

		while( !quit ) {

			SDL_Event event;

			while( SDL_PollEvent( &event ) ) {
				switch( event.type ) {
				case SDL_WINDOWEVENT:
					if( event.window.event == SDL_WINDOWEVENT_RESIZED )
						glViewport( 0, 0, event.window.data1, event.window.data2 );
					break;

				default: quit = aGame.input( event ); break;
				}
			}

			if( quit )
				break;

			if( SDL_GetTicks64() > ( game_tick + 16 ) ) {

				aGame.update( SDL_GetTicks64() - game_tick );

				sdl_wrapper.clear_window();

				aGame.draw();

				sdl_wrapper.display_window();

				game_tick = SDL_GetTicks64();
			}
		}

		return 0;
	};

private:
	SDL_Wrapper sdl_wrapper;
	T aGame;
	SetupParams params;
};

class BlankGame : public Game
{
	SetupParams get_params() override { return SetupParams( { "Blank Game", 640, 480, 0, SDL_RENDERER_ACCELERATED } ); }
	void setup() override {}
	bool process_event( SDL_Event &event ) override { return event.type == SDL_QUIT; }
	void update_state( uint64_t elapsed_time ) override{};
	void draw_frame() override{};
};