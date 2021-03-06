/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "shell.h"

namespace ucsh {

typedef const char*  source_type;
typedef source_type& source_ref;
typedef CommandGroup result_type;
typedef result_type& result_ref;
typedef string&      string_buf;

typedef enum { END, CONT, ERRTOK, ERRPAIR, ERRSYN } status_type;

cstr_t      parse_env  (source_ref);
cstr_t      parse_var  (source_type); // do not changes input
status_type parse_all  (source_ref, result_ref);
status_type parse_word (source_ref, result_ref, string_buf);

inline bool ismagic(const char c) {
	switch (c) {
	case '*': case '?': case '[': case '\\':
		return true;
	}
	return false;
}

inline bool isterm(const char c) {
	switch (c) {
	case ' ': case ';': case '|': case '&':
	case '<': case '>': case '\0':
		return true;
	}
	return false;
}

}

#endif
