/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef UCSH_H_
#define UCSH_H_

#include "editline.h"
#include "shell.h"

/*
 * The main_loop() and run_argv() for the ucsh. Note that you
 * need to setup() the ucsh::Shell before args being changed.
 */

namespace ucsh {

int main_loop();
int run_argv(argc_t, argv_t);

}

#endif
