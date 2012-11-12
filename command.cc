/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "command.h"
#include "shell.h"
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

// print all unexpended commands, one command per line
int ucsh::print(CommandGroup& o) {
	for (CommandGroup::iterator i = o.begin(); i != o.end(); ++i)
		std::cout << *i;
	return 0;
}

int ucsh::execute(CommandGroup& o) {
	int st, wst;
	int ofd = 0;
	for (CommandGroup::iterator i = o.begin(); i != o.end(); ++i) {
		st = 0;
		Glob cbuf;
		for (size_t j = 0; j < i->args.size(); ++j)
			if (GLOB_NOMATCH == cbuf.match(i->args[j],
					GLOB_NOMAGIC | GLOB_TILDE |
					(j ? GLOB_APPEND : 0))) {
				P_ERRF(i->args[0].c_str(), "No match");
				st = 1;
				break;
			}
		if (st) continue; // resume when glob fails
		if (Shell::isbuiltin(*cbuf.pathv))
			// turn C call status to exit status
			st = !!Shell::call(cbuf.pathc, cbuf.pathv);
		else {
			int fd[2]; // initialize pipe
			if (i->opt == PIPE and pipe(fd))
				X_ERR("pipe()", return);
			pid_t pid = fork();
			if (pid < 0)
				X_ERR("fork()", return);
			if (!pid) {
				bool backward = i != o.begin();
				if (i->opt == PIPE and
				    (not backward or (i-1)->opt != PIPE)) {
					dup2(fd[1], 1);
					close(fd[1]);
				} else if (i->opt == PIPE and
				    backward and (i-1)->opt == PIPE) {
					dup2(ofd, 0);
					close(ofd);
					dup2(fd[1], 1);
					close(fd[1]);
				} else if (i->opt != PIPE and
				    (not backward or (i-1)->opt == PIPE)) {
					dup2(ofd, 0);
					close(ofd);
				} else if (RDR_R <= i->opt and i->opt <= RDR_A) {
					Glob rbuf;
					if (GLOB_NOMATCH == rbuf.match((++i)->args[0],
							GLOB_NOMAGIC | GLOB_TILDE)) {
						P_ERRF(i->args[0].c_str(), "No match");
						exit(1);
					}
					if (i->args.size() > 1 or rbuf.pathc > 1) {
						P_ERRF(*cbuf.pathv, "Too many targets");
						exit(1);
					}
					int rf;
					char* fn = *rbuf.pathv;
					switch ((i-1)->opt) {
					case RDR_R:
						if ((rf = open(fn, O_RDONLY)) < 0)
							X_ERR(fn, exit);
						dup2(rf, 0);
						close(rf);
						break;
					case RDR_W:
						if ((rf = creat(fn, 0666)) < 0)
							X_ERR(fn, exit);
						goto rdr_to;
					case RDR_A:
						if ((rf = open(fn, O_WRONLY | O_APPEND)) < 0)
							X_ERR(fn, exit);
					rdr_to:
						dup2(rf, 1);
						close(rf);
					default:;
					}
				}
				if (execvp(*cbuf.pathv, cbuf.pathv)) {
					if (errno == ENOENT) // "no fuch file" in $PATH
						P_ERRF(*cbuf.pathv, "Command not found");
					else perror(*cbuf.pathv);
					exit(1); // when exec failed
				}
			} else switch (i->opt) {
			case JOBS:
				fprintf(stderr, "[%8d]\n", pid);
				waitpid(pid, &wst, WNOHANG); // still need to wait
				st = WEXITSTATUS(wst);
				break;
			case PIPE:
				ofd = fd[0];  // preserved old output
				close(fd[1]); // send EOF to input
			case RDR_R: case RDR_W: case RDR_A:
				++i;
			default:
				waitpid(pid, &wst, WCONTINUED);
				st = WEXITSTATUS(wst);
			}
		}
	}
	return st;
}

