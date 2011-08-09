cmake_minimum_required(VERSION 2.6)
include(CheckCXXSourceCompiles)
project(ucsh)

option(USE_HIST_FILE "Saves history to ~/.ucsh_hist" ON)

check_cxx_source_compiles(
	"#include <tr1/unordered_map>
	int main() {
		std::tr1::unordered_map<int, int> m;
		return 0;
	}"
	USE_HASH_MAP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
if (USE_HASH_MAP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_HASH_MAP")
endif (USE_HASH_MAP)
if (USE_HIST_FILE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_HIST_FILE")
endif (USE_HIST_FILE)

find_path(EDITLINE_INCLUDE_DIR histedit.h)
find_library(EDITLINE_LIBRARY NAMES edit)

if (EDITLINE_INCLUDE_DIR AND EDITLINE_LIBRARY)
	set(EDITLINE_FOUND TRUE)
endif (EDITLINE_INCLUDE_DIR AND EDITLINE_LIBRARY)

if (EDITLINE_FOUND)
	message(STATUS "Found BSD editline: ${EDITLINE_LIBRARY}")
else (EDITLINE_FOUND)
	message(FATAL_ERROR "Could not find BSD editline")
endif (EDITLINE_FOUND)

include_directories(${EDITLINE_INCLUDE_DIR})
set(LIBS ${LIBS} ${EDITLINE_LIBRARY})

add_executable(ucsh
	builtin.cc "command.cc" main.cc parser.cc shell.cc ucsh.cc)
target_link_libraries(ucsh ${LIBS})
