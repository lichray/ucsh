/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "command.h"

namespace ucsh {

typedef const char*  source_type;
typedef source_type& source_ref;
typedef CommandGroup result_type;
typedef result_type& result_ref;
typedef string&&     string_buf;

typedef enum { END, CONT, ERRTOK, ERRPAIR, ERRSYN } status_type;

char*       parse_env  (source_ref);
const char* parse_var  (source_type&&); // will still changes input
status_type parse_all  (source_ref, result_ref);
status_type parse_word (source_ref, result_ref, string_buf = "");

inline bool ismagic(const char c)
{ return c == '*' or c == '?' or c == '[' or c == '\\'; }

inline bool isterm(const char c)
{ return c == ' ' or c == ';' or c == '|' or c == '&' or !c; }

}

#endif
