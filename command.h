/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <vector>
#include <iostream>
#include <glob.h>

using std::string;
using std::vector;

namespace ucsh {

// operators after PIPE are all binary
typedef enum { NOOP, COMB, JOBS, PIPE, RDR_R, RDR_W, RDR_A } op_type;

// the class that represents a command and it's tail operator
struct Command {
	typedef vector<string> _Rep;
	typedef _Rep::iterator iterator;
	typedef _Rep::const_iterator const_iterator;

	Command() : args(_Rep()), opt(NOOP) {}

	_Rep args;
	op_type opt;
};

// inheriting from vector, warning under -Weffc++
struct CommandGroup : vector<Command> {
	typedef vector<Command> _Base;

	// checks whether these commands are well ended
	bool integrated() const
	{ return this->back().opt < PIPE; }

	int execute();
	int print();
};

struct Glob {
	Glob() : _rep(), pathc(_rep.gl_pathc), matchc(_rep.gl_matchc),
	offs(_rep.gl_offs), flags(_rep.gl_flags), pathv(_rep.gl_pathv) {}

	int match(char const* pattern, int newflags = 0)
	{ return glob(pattern, newflags, NULL, &_rep); }

	int match(string const& pattern, int newflags = 0)
	{ return match(pattern.c_str(), newflags); }

	~Glob() throw() { globfree(&_rep); }

private:
	glob_t _rep;
	Glob(Glob const&);
	Glob& operator=(Glob&);

public:
	size_t& pathc;
	size_t& matchc;
	size_t& offs;
	int&    flags;
	char**& pathv;
};

// prints a Command with ostream like cout, without %opt
inline std::ostream& operator<<(std::ostream& os, ucsh::Command& c) {
	for (ucsh::Command::const_iterator i = c.args.begin();
			i != c.args.end(); ++i)
		os << *i << ' ';
	os << '\n';
	return os;
}

}

#endif
