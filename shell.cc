/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "shell.h"

namespace ucsh {

argc_t Shell::argc = 0;
char** Shell::argv = NULL;
cstr_t Shell::_ws  = "";
Shell::cmds_t Shell::cmds;

}
