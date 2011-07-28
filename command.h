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

typedef enum { WORD, SINGLE, DOUBLE, COMB, PIPE, JOBS } symbol_type;

// the class that represents a command and it's tail operator
struct Command {
	typedef vector<string> _Rep;
	typedef _Rep::iterator iterator;
	typedef _Rep::const_iterator const_iterator;

	Command() : args(_Rep()), opt(WORD) {}

	_Rep args;
	symbol_type opt; // SINGLE & DOUBLE are not valid operators
};

// inheriting from vector, warning under -Weffc++
struct CommandGroup : vector<Command> {
	typedef vector<Command> _Base;

	// checks whether these commands are well ended
	bool integrated() const
	{ return this->back().opt != PIPE; } // the only binary opt

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
