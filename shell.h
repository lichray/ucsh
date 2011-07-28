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

	typedef int (*cmd_ptr)(argc_t, argv_t);
	typedef T_TABLE(cmd_ptr) cmds_t;
	typedef T_TABLE(string) vars_t;

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
		cmds["set"] = builtin::set;
		cmds["unset"] = builtin::unset;

		if (!::getenv("PS1"))
			setvar("PS1", "ucsh> ");
		if (!::getenv("PS2"))
			setvar("PS2", "? ");
	}

	// gets an environment variable, fallbacks to empty string
	static cstr_t getenv(cstr_t k) {
		char* v = ::getenv(k);
		return v ? v : _ws;
	}

	// sets/unsets an internal shell variable
	static int setvar(cstr_t k, cstr_t v)
	{ return v ? (vars[k] = v, 0) : -1; }

	static int unsetvar(cstr_t k)
	{ return vars.erase(k) ? 0 : -1; }

	// gets a shell variable, fallbacks to environment variable
	static cstr_t getvar(cstr_t k) {
		if (isdigit(*k)) {
			argc_t c = atoi(k);
			return c < argc ? argv[c] : _ws;
		}
		if (vars.count(k))
			return vars[k].c_str();
		return getenv(k);
	}

	// check whether it's a built-in command
	static bool isbuiltin(cstr_t s)
	{ return cmds.count(s); }

	// call a built-in command with its name
	static int call(argc_t c, argv_t v)
	{ return cmds[*v](c, v); }

private:
	static argc_t argc;
	static char** argv;
	static cmds_t cmds;
	static vars_t vars;
	static cstr_t  _ws;
};

}

#endif
