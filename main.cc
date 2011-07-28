/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "ucsh.h"

#define __M_NAME__ "ucsh"
#define __M_VERSION__ 1.0

#define __M_BANNER__ \
"                     _      \n"\
" _ __ ___  _   _ ___| |__   \n"\
"| '_ ` _ \\| | | / __| '_ \\  \n"\
"| | | | | | |_| \\__ \\ | | | \n"\
"|_| |_| |_|\\__, |___/_| |_| \n"\
"           |___/            \n"

#define __M_USAGE__ \
__M_NAME__ " Copyright 2010 Zhihao Yuan <lichray@gmail.com> \n" \
"Version: 1.0, date: 13 Oct 2010 \n" \
"License: <http://www.freebsd.org/copyright/freebsd-license.html> \n\n" \
"Usage:  ucsh [-hvVc] [CMD...]\n" \
" -h      show help message\n" \
" -v      show program version\n" \
" -c CMD  execute the command\n" \


int main(int argc, char* argv[]) {
	using ucsh::Shell;

	Shell::setup(argc, argv);
	int ch;
	while ((ch = getopt(argc, argv, "hvVc")) != -1)
		switch (ch) {
		case 'h':
			puts(__M_USAGE__);
			return 0;
		case 'v':
			printf(__M_NAME__ " %.1f\n", __M_VERSION__);
			return 0;
		case 'V':
			puts(__M_BANNER__);
			return 0;
		case 'c':
			return ucsh::run_argv(argc-optind, argv+optind);
		default:
			puts(__M_USAGE__);
			exit(1);
		}

	if (!getenv("PS1"))
		Shell::setenv("PS1", "$SHELL> ");
	if (!getenv("PS2"))
		Shell::setenv("PS2", "? ");
	Shell::setenv("SHELL", __M_NAME__);

	return ucsh::main_loop();
}

