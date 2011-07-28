/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef BUILTIN_H_
#define BUILTIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

namespace ucsh {

typedef const char* cstr_t;
typedef int         argc_t;
typedef char**const argv_t;

// the macros that print error messages; formats like perror().
#define P_ERR(s) (fputs((s ".\n"), stderr))
#define P_ERRF(c, s) (fprintf(stderr, "%s: " s ".\n", c))
#define P_ERRS(s) (fprintf(stderr, "%s: " s ".\n", __func__))

/*
 * A built-in command works like a main() function: it accepts
 * the command arguments and returns an exit status. But it is
 * not process, and it has no file descriptors, so it just
 * regards the pipe and the job operators as combinators.
 *
 * The built-ins work almost the same as that in csh(1), but
 * without the I/O support. The commands that require the shell
 * storage or input/output are not implemented yet.
 */
namespace builtin {

#define D_CMD(tok) int tok(argc_t argc, argv_t argv)

D_CMD(cd);
D_CMD(exec);
D_CMD(exit);
D_CMD(quit);
D_CMD(setenv);
D_CMD(unsetenv);

// Shell storage required
D_CMD(set);
D_CMD(unset);
// Pipe on builtin required
D_CMD(echo);
D_CMD(alias);

}}

#endif
