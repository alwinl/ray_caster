/*
 * testglm.cc Copyright 2024 Alwin Leerling dna.leerling@gmail.com
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

#include "testglm.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestGLM );

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>

namespace
{

struct Model {
	float x_pos;
	float y_pos;
	float angle;
	int scale;
};

glm::mat4 get_trans_matrix( Model &model )
{
	constexpr glm::vec3 z_axis = glm::vec3( 0.0F, 0.0F, 1.0F );
	const glm::vec3 position( model.x_pos, model.y_pos, 0.0 );

	glm::mat4 trans = glm::mat4( 1.0F );

	trans = glm::translate( trans, position );
	trans = glm::rotate( trans, model.angle, z_axis );
	trans = glm::scale( trans, glm::vec3( model.scale, model.scale, 1 ) );

	return trans;
}

glm::mat4 get_new_trans_matrix( Model &model )
{
	const auto fscale = static_cast<float>( model.scale );

	return glm::mat4( {
		{ fscale * std::cos( model.angle ), fscale * std::sin( model.angle ), 0.0F, 0.0F },
		{ -fscale * std::sin( model.angle ), fscale * std::cos( model.angle ), 0.0F, 0.0F },
		{ 0.0F, 0.0F, 1.0F, 0.0F },
		{ model.x_pos, model.y_pos, 0.0F, 1.0F },
	} );
}

} // namespace

void TestGLM::check_new_trans_calculation()
{
	Model model{};

	srand( static_cast<unsigned>( time( nullptr ) ) );

	for( int test = 0; test < 10000; ++test ) {
		model.x_pos = static_cast<float>( std::fmod( rand(), 800.0F ) );
		model.y_pos = static_cast<float>( std::fmod( rand(), 800.0F ) );
		model.angle = glm::radians( static_cast<float>( std::fmod( rand(), 360.0F ) ) );
		model.scale = rand() % 50;

		// std::cerr << "{ " << model.x_pos << ", " << model.y_pos << ", " << model.angle << ", " << model.scale << "}"
		// << std::endl;
		glm::mat4 old_result = get_trans_matrix( model );
		glm::mat4 new_result = get_new_trans_matrix( model );

		for( const int first : { 0, 1, 2, 3 } )
			for( const int second : { 0, 1, 2, 3 } )
				CPPUNIT_ASSERT( std::fabs( old_result[first][second] - new_result[first][second] ) < 0.000001 );
	}
}