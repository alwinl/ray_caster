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

#include <string>

#include <SDL2/SDL.h>

struct SetupParams {
	std::string title;
	int width;
	int height;
	uint32_t flags; // potential to make this an enum class
	int rendererFlags = SDL_RENDERER_ACCELERATED;
};

class Game
{
public:
	Game() = default;
	virtual ~Game() = default;

	SetupParams make_setup() { return get_params(); }
	void initialise() { setup(); }
	bool input( SDL_Event &event ) { return process_event( event ); };
	void update( uint64_t elapsed_time ) { update_state( elapsed_time ); }
	void draw( SDL_Renderer *window )
	{
		this->window = window;
		draw_frame( window );
	}

protected:
	virtual SetupParams get_params() = 0;
	virtual void setup() = 0;
	virtual bool process_event( SDL_Event &event ) = 0;
	virtual void update_state( uint64_t elapsed_time ) = 0;
	virtual void draw_frame( SDL_Renderer *window ) = 0;

	SDL_Renderer *window = nullptr;
};

template <typename T> class GameWrapper
{
public:
	int run()
	{
		SDL_Init( SDL_INIT_EVERYTHING );

		SetupParams params = aGame.make_setup();

		SDL_Window *window = SDL_CreateWindow( params.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
											   params.width, params.height, params.flags );
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );
		SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, params.rendererFlags );

		aGame.initialise();

		uint64_t game_tick = SDL_GetTicks64();

		bool quit = false;

		while( !quit ) {

			SDL_Event event;

			while( SDL_PollEvent( &event ) ) quit = aGame.input( event );

			if( quit )
				break;

			if( SDL_GetTicks64() > ( game_tick + 16 ) ) {

				aGame.update( SDL_GetTicks64() - game_tick );

				SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
				SDL_RenderClear( renderer );

				aGame.draw( renderer );

				SDL_RenderPresent( renderer );

				game_tick = SDL_GetTicks64();
			}
		}

		SDL_DestroyWindow( window );
		SDL_DestroyRenderer( renderer );

		SDL_Quit();

		return 0;
	};

private:
	T aGame;
};

class BlankGame : public Game
{
	SetupParams get_params() override { return SetupParams( { "Blank Game", 640, 480, 0, SDL_RENDERER_ACCELERATED } ); }
	void setup() override {}
	bool process_event( SDL_Event &event ) override { return event.type == SDL_QUIT; }
	void update_state( uint64_t elapsed_time ) override{};
	void draw_frame( SDL_Renderer *window ) override{};
};