/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "command.h"
#include "shell.h"
#include <glob.h>
#include <errno.h>
#include <sys/wait.h>

// print all unexpended commands, one command per line
int ucsh::CommandGroup::print() {
	for (iterator i = this->begin(); i != this->end(); ++i)
		std::cout << *i;
	return 0;
}

int ucsh::CommandGroup::execute() {
	int st = 0, wst = 0;
	int ofd = 0;
	for (iterator i = this->begin(); i != this->end(); ++i) {
		glob_t cbuf;
		for (size_t j = 0; j < i->args.size(); ++j)
			if (GLOB_NOMATCH == glob(i->args[j].c_str(),
					GLOB_NOMAGIC | GLOB_TILDE |
					(j ? GLOB_APPEND : 0), NULL, &cbuf)) {
				// fails only when magic char has no match
				globfree(&cbuf);
				P_ERRF(i->args[0].c_str(), "No match");
				st = 1;
				break;
			}
		if (st) continue; // resume when glob fails
		if (Shell::isbuiltin(*cbuf.gl_pathv))
			// turn C call status to exit status
			st = !!Shell::call(cbuf.gl_pathc, argv_t(cbuf.gl_pathv));
		else {
			int fd[2]; // initialize pipe
			if (i->opt == PIPE and pipe(fd)) perror("pipe()");
			pid_t pid = fork();
			if (pid < 0) perror("fork()");
			else if (!pid) {
				if (i->opt == PIPE and (i-1)->opt != PIPE) {
					close(fd[0]);
					dup2(fd[1], 1);
					close(fd[1]);
				} else if (i->opt == PIPE and (i-1)->opt == PIPE) {
					dup2(ofd, 0);
					close(ofd);
					dup2(fd[1], 1);
					close(fd[1]);
				} else if (i->opt != PIPE and (i-1)->opt == PIPE) {
					close(fd[1]);
					dup2(ofd, 0);
					close(ofd);
				}
				if(execvp(*cbuf.gl_pathv, cbuf.gl_pathv)) {
					if (errno == ENOENT) // "no fuch file" in $PATH
						P_ERRF(*cbuf.gl_pathv, "Command not found");
					else perror(*cbuf.gl_pathv);
					exit(1); // when exec failed
				}
			} else if (i->opt == JOBS) {
				printf("[%8d]\n", pid);
				waitpid(pid, &wst, WNOHANG); // still need to wait
				st = WEXITSTATUS(wst);
			} else if (i->opt == PIPE) {
				ofd = fd[0];  // preserved old output
				close(fd[1]); // send EOF to input
			} else {
				waitpid(pid, &wst, WCONTINUED);
				st = WEXITSTATUS(wst);
			}
		}
		globfree(&cbuf);
	}
	return st;
}

