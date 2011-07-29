/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include <sys/param.h>
#include "builtin.h"
#include "shell.h"

extern char** environ;

namespace ucsh {
namespace builtin {

#define G_ARG(a, b) { \
	if (argc-1 < a) \
		return (fprintf(stderr,"%s: Too few arguments.\n", __func__),-1); \
	if (b >= 0 && (argc-1 > b)) \
		return (fprintf(stderr,"%s: Too many arguments.\n",__func__),-1); \
}

D_CMD(cd) {
	G_ARG(0, 1);
	cstr_t path = argc > 1 ? argv[1] : getenv("HOME");
	int st = chdir(path);
	char cwd[PATH_MAX+1];
	if (!st) ::setenv("PWD", getcwd(cwd, PATH_MAX), 1);
 	// different from csh(1):
	// we shows "Bad Address" when you has no home directory.
	else perror(path ? path : __func__);
	return st;
}

D_CMD(exec) {
	G_ARG(1, -1);
	int st = execvp(argv[1], const_cast<char* const*>(argv+1));
	if (st) perror(argv[1]); // stays in the shell when exec fails
	return st;
}

D_CMD(exit) {
	G_ARG(0, 1);
	::exit(argc > 1 ? atoi(argv[1]) : 0);
	return 0;
}

D_CMD(quit) {
	G_ARG(0, 0); // quit command equals to exit 0
	return (::exit(0), 0);
}

D_CMD(setenv) {
	G_ARG(0, 2);
	if (argc < 2) {
		for (char** ep = environ; *ep; ep++)
			puts(*ep);
		return 0;
	}
	return ::setenv(argv[1], argc > 2 ?  argv[2] : "", 1);
}

D_CMD(unsetenv) {
	G_ARG(1, 1);
	return ::unsetenv(argv[1]);
}

D_CMD(set) {
	G_ARG(0, 2);
	return argc < 2 ? Shell::printvars() :
		Shell::setvar(argv[1], argc > 2 ? argv[2] : "");
}

D_CMD(unset) {
	G_ARG(1, 1);
	return Shell::unsetvar(argv[1]);
}

#undef G_ARG
#undef D_CMD

}}

