
#  CMakeLists.txt Copyright 2024 Alwin Leerling dna.leerling@gmail.com

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.

find_package( PkgConfig REQUIRED )
pkg_check_modules( CPPUNIT REQUIRED cppunit )

include_directories( ${CPPUNIT_INCLUDE_DIRS})
link_directories( ${CPPUNIT_LIBRARY_DIRS} )
link_libraries( ${CPPUNIT_LIBRARIES} )

file( GLOB SOURCES "*.cc" )
add_executable( test_runner ${SOURCES} )

target_compile_features( test_runner PRIVATE cxx_std_20)

find_package(glm CONFIG REQUIRED)
# for linux we have to link glm::glm. For windows its glm. Go figure..
if( UNIX )
target_link_libraries(test_runner PRIVATE glm::glm)
endif(UNIX)

if(WIN32)
target_link_libraries(test_runner PRIVATE glm)
endif(WIN32)

target_link_libraries( test_runner PRIVATE SDL2_Wrapper )


# add_test( testsdl2wrapper::RunBlankGame test_runner testsdl2wrapper::RunBlankGame )
# add_test( testsdl2wrapper::ColouredBackground test_runner testsdl2wrapper::ColouredBackground )
# add_test( TestTilepainting::RunTileGame test_runner TestTilepainting::RunTileGame )