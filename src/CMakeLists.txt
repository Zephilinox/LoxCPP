cmake_minimum_required (VERSION 3.15)

project(LoxCPP)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT LoxCPP)

set(LoxCPP_FILES
	"main.cpp"

	"Interpreter.hpp"
	"Interpreter.cpp"

	"LoxCPP/Lexer.hpp"
	"LoxCPP/Lexer.cpp"
	
	"LoxCPP/Parser.hpp"
	"LoxCPP/Parser.cpp"

	"LoxCPP/Token.hpp"
	"LoxCPP/Expressions.hpp"
)

file(GLOB_RECURSE LoxCPP_FILES_GLOBBED LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.cpp *.hpp)

#message(STATUS "LoxCPP_FILES = ${LoxCPP_FILES}")
#message(STATUS "LoxCPP_FILES_GLOBBED = ${LoxCPP_FILES_GLOBBED}")

foreach (file IN LISTS LoxCPP_FILES_GLOBBED)
	if (NOT ${file} IN_LIST LoxCPP_FILES)
		message("Warning - Globbed file ${file} was not listed in LoxCPP_FILES")
	endif()
endforeach()

foreach (file IN LISTS LoxCPP_FILES)
	if (NOT ${file} IN_LIST LoxCPP_FILES_GLOBBED)
		message("Warning - File ${file} was not listed in LoxCPP_FILES_GLOBBED")
	endif()
endforeach()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "src" FILES ${LoxCPP_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

#message("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
include(PreTargetSettings)
add_executable(LoxCPP ${LoxCPP_FILES})
include(PostTargetSettings)

if (UNIX)
	set(UNIX_LIBS pthread)
endif()

target_link_libraries(LoxCPP
	${UNIX_LIBS}
)

target_include_directories(LoxCPP PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)