# NINJA GAIDEN Master Collection NM Plugin by Nozomi Miyamori
# is marked with CC0 1.0. This file is a part of NINJA GAIDEN
# Master Collection NM Plugin.

cmake_minimum_required(VERSION 3.20)
project(ngs2-nm-plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OPTIMIZE_DEPENDENCIES on)
set(CMAKE_SHARED_MODULE_SUFFIX ".dll")

add_library(util INTERFACE)
target_include_directories(util INTERFACE src)

link_libraries(util)

add_library(core MODULE src/core/main.cpp)
set_property(TARGET core PROPERTY PREFIX "")
set_property(TARGET core PROPERTY OUTPUT_NAME "d3d9")

add_library(loader MODULE src/loader/main.cpp)
add_library(effect MODULE src/effect/main.cpp)
add_library(combat MODULE src/combat/main.cpp)
set_property(TARGET
	loader
	effect
	combat
	PROPERTY PREFIX "nm")

#install(TARGETS core DESTINATION .)
#install(TARGETS loader DESTINATION ./plugin)
install(TARGETS effect DESTINATION ./mods/bin)
install(TARGETS combat DESTINATION ./mods/bin)
