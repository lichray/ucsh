/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "ucsh.h"
#include <sstream>

namespace ucsh {

int main_loop() {
	source_type s;
	int st = 0;
	EditLine el("ucsh");

	while ((s = el.readline("PS1"))) {
		std::stringstream hs;
		cstr_t p = s; // the original pointer
		while (isspace(*s)) ++s;
		if (*s) hs << s;
		else continue;
		CommandGroup t;
	prompt2: // loop to acquire the continued commands
		status_type pt = parse_all(s, t);
		switch (pt) {
			case CONT    :
				if (!(s = el.readline("PS2"))) continue;
				p = s;
				while (isspace(*s)) ++s;
				if (*s) hs << '\n' << p;
				goto prompt2;
			case ERRTOK  : P_ERR("Unknown token");  break;
			case ERRPAIR : P_ERR("Unpaired quote"); break;
			case ERRSYN  : P_ERR("Syntax error");   break;
			case END     : st = t.execute();
		}
		el.enter(hs.str().c_str());
	}
	putchar('\n');
	return st;
}

int run_argv(argc_t argc, argv_t argv) {
	string b;
	CommandGroup t;

	for (int i = 0; i < argc; ++i) {
		b.append(argv[i]);
		b.push_back(' ');
	}
	source_type s = b.c_str();
	if (parse_all(s, t) == END)
		return t.execute();
	P_ERR("Unfinished command or Syntax error");
	return 1;
}

}
