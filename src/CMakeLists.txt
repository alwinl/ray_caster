
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
find_package(glm CONFIG REQUIRED)

add_executable(ray_caster
	main.cc
	tile_painting_game.cc
	player.cc
)

target_link_libraries( ray_caster PRIVATE SDL2_Wrapper )

# for linux we have to link glm::glm. For windows its glm. Go figure..
target_link_libraries(ray_caster PRIVATE glm::glm)
#target_link_libraries(ray_caster PRIVATE glm)

