/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef SHELL_H_
#define SHELL_H_

#include <ctype.h>
#include "command.h"
#include "builtin.h"

// if your compiler does not support c++0x,
// it won't help even it supports hash_map.
#ifdef USE_HASH_MAP
	#include <tr1/unordered_map>
	#define T_TABLE(t) std::tr1::unordered_map<string, t>
#else
	#include <map>
	#define T_TABLE(t) std::map<string, t>
#endif

namespace ucsh {

struct Shell {
#define _WS (const_cast<char*>(_ws))

	typedef int (*cmd_ptr)(argc_t, argv_t);
	typedef T_TABLE(cmd_ptr) cmds_t;

	// initialize command-line options and built-ins
	static void setup(argc_t c, argv_t v) {
		argc = c;
		argv = v;
		cmds["cd"  ] = builtin::cd;
		cmds["exec"] = builtin::exec;
		cmds["exit"] = builtin::exit;
		cmds["quit"] = builtin::quit;
		cmds["setenv"] = builtin::setenv;
		cmds["unsetenv"] = builtin::unsetenv;
		cmds["q"] = builtin::quit; // alias
	}

	// force set an environment variable
	static int setenv(cstr_t k, cstr_t v)
	{ return ::setenv(k, v, 1); }

	// gets an environment variable, fallbacks to empty string
	static char* getenv(cstr_t k) {
		char* v = ::getenv(k);
		return v ? v : _WS;
	}

	// gets a shell variable, fallbacks to environment variable
	static char* getvar(cstr_t k) {
		if (isdigit(*k)) {
			argc_t c = atoi(k);
			return c <= argc ? argv[c] : _WS;
		}
		return getenv(k);
	}

	// check whether it's a built-in command
	static bool isbuiltin(cstr_t s)
	{ return cmds.count(s); }

	// call a built-in command with its name
	static int call(argc_t c, argv_t v)
	{ return cmds[*v](c, v); }

#undef _WS

private:
	static argc_t argc;
	static char** argv;
	static cmds_t cmds;
	static cstr_t  _ws;
};

}

#endif
