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
#include <fcntl.h>

// print all unexpended commands, one command per line
int ucsh::CommandGroup::print() {
	for (iterator i = this->begin(); i != this->end(); ++i)
		std::cout << *i;
	return 0;
}

int ucsh::CommandGroup::execute() {
	int st, wst;
	int ofd = 0;
	for (iterator i = this->begin(); i != this->end(); ++i) {
		st = 0;
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
					dup2(fd[1], 1);
					close(fd[1]);
				} else if (i->opt == PIPE and (i-1)->opt == PIPE) {
					dup2(ofd, 0);
					close(ofd);
					dup2(fd[1], 1);
					close(fd[1]);
				} else if (i->opt != PIPE and (i-1)->opt == PIPE) {
					dup2(ofd, 0);
					close(ofd);
				} else if (RDR_R <= i->opt and i->opt <= RDR_A) {
					glob_t rbuf;
					if (GLOB_NOMATCH == glob((++i)->args[0].c_str(),
							GLOB_NOMAGIC | GLOB_TILDE,
							NULL, &rbuf)) {
						globfree(&rbuf);
						P_ERRF(i->args[0].c_str(), "No match");
						exit(1);
					}
					if (i->args.size() > 1 or rbuf.gl_pathc > 1) {
						P_ERRF(*cbuf.gl_pathv, "Too many targets");
						exit(1);
					}
					int rf;
					char* fn = *rbuf.gl_pathv;
					switch ((i-1)->opt) {
					case RDR_R:
						if ((rf = open(fn, O_RDONLY)) < 0)
							X_ERR(fn);
						dup2(rf, 0);
						close(rf);
						break;
					case RDR_W:
						if ((rf = creat(fn, 0666)) < 0)
							X_ERR(fn);
						goto rdr_to;
					case RDR_A:
						if ((rf = open(fn, O_WRONLY | O_APPEND)) < 0)
							X_ERR(fn);
					rdr_to:
						dup2(rf, 1);
						close(rf);
					default:;
					}
					globfree(&rbuf);
				}
				if (execvp(*cbuf.gl_pathv, cbuf.gl_pathv)) {
					if (errno == ENOENT) // "no fuch file" in $PATH
						P_ERRF(*cbuf.gl_pathv, "Command not found");
					else perror(*cbuf.gl_pathv);
					exit(1); // when exec failed
				}
			} else switch (i->opt) {
			case JOBS:
				printf("[%8d]\n", pid);
				waitpid(pid, &wst, WNOHANG); // still need to wait
				st = WEXITSTATUS(wst);
				break;
			case PIPE:
				ofd = fd[0];  // preserved old output
				close(fd[1]); // send EOF to input
				break;
			case RDR_R: case RDR_W: case RDR_A:
				++i;
			default:
				waitpid(pid, &wst, WCONTINUED);
				st = WEXITSTATUS(wst);
			}
		}
		globfree(&cbuf);
	}
	return st;
}

