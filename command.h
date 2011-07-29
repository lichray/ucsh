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
using std::string;
using std::vector;

namespace ucsh {

typedef enum { NOOP, COMB, PIPE, JOBS, RDR_R, RDR_W, RDR_A } op_type;

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
	bool integrated() const {
		switch(this->back().opt) {
		case NOOP: case COMB: case JOBS:
			return true;
		default:
			return false;
		}
	}

	int execute();
	int print();
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
