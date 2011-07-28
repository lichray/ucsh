cmake_minimum_required(VERSION 2.6)
project(ucsh)

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
