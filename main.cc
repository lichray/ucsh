/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "ucsh.h"

#define __M_NAME__ "ucsh"
#define __M_VERSION__ "1.1"

#define __M_BANNER__ \
"                _     \n" \
" _   _  ___ ___| |__  \n" \
"| | | |/ __/ __| '_ \\ \n" \
"| |_| | (__\\__ \\ | | |\n" \
" \\__,_|\\___|___/_| |_|\n"

#define __M_USAGE__ \
__M_NAME__ " " __M_VERSION__ " (zy) 2011/07/28\n" \
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
			puts(__M_NAME__ " " __M_VERSION__);
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

	return ucsh::main_loop();
}

