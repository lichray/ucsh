/*-
 * Copyright (c) 2010, 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#include "parser.h"

namespace ucsh {

/*
 * The recursive descent parsers take an input %i and construct
 * an output %t. When a parser is terminated, it returns a status -
 * 0 means it has done, 1 means it reaches the end-of-string without
 * seen its terminator char, others are errors.
 *
 * A parser with the third parameter, buffer, can hold its data
 * for further parsing. These parsers have their own call circle.
 *
 * @param source_ref i  pointer/iterator to the source string
 *                      it will be changed in case of copying
 * @param result_ref t  reference to an abstract syntax type
 * @param string_buf b 
 * @return status_type  END/CONT/ERR*
 */

// the macros that define/call a parser w/o buffer
#define D_PARSE(tok) \
	status_type parse_##tok(source_ref i, result_ref t)
#define D_PARSE_BUF(tok) \
	status_type parse_##tok(source_ref i, result_ref t, string_buf b)
#define C_PARSE(tok) \
	(parse_##tok(i, t))
#define C_PARSE_BUF(tok) \
	(parse_##tok(i, t, b))

/*
 * parse_word(), parse_raw(), parse_str() consist the command-line
 * parameters call circle. parse_word() is the entrance. These parsers
 * handle the argument combined by raw strings (single quoted), rich
 * strings (double quoted), and bare words.
 *
 * Raw string prevents the magic chars from being expended by glob();
 * Rich string protects the magic chars also, but it expends the
 * environment variables;
 * Bare word expends the environment variables, and it's the only place
 * where the magic chars works.
 */

D_PARSE_BUF(raw) {
	char c;
	while ((c = *i++)) {
		if (c == '\'') return C_PARSE_BUF(word);
		if (ismagic(c)) b.push_back('\\');
		b.push_back(c);
	}
	return ERRPAIR;
}

D_PARSE_BUF(str) {
	char c;
	while ((c = *i++)) {
		if (c == '\"') return C_PARSE_BUF(word);
		if (ismagic(c)) b.push_back('\\');
		if (c == '$')
			b.append(parse_env(i));
		else b.push_back(c);
	}
	return ERRPAIR;
}

D_PARSE_BUF(word) {
	char c;
	while (1) {
		c = *i++;
		if (c == '\'') return C_PARSE_BUF(raw);
		if (c == '\"') return C_PARSE_BUF(str);
		if (isterm(c)) {
			t.last().args.push_back(b);
			return (--i, C_PARSE(all));
		}
		if (c == '$')
			b.append(parse_env(i));
		else {
			// "\\\n" is a special terminator
			if (c == '\\' and ((c = *i++) == '\n' or !c)) {
				if (b.size())
					t.last().args.push_back(b);
				return c ? C_PARSE(all) : CONT;
			}
			b.push_back(c);
		}
	}
}

#define parse_op(op) do { \
	if (t.last().args.empty()) return ERRSYN; \
	t.last().opt = op; \
	return C_PARSE(all); \
} while(0)

D_PARSE(all) {
	if (t.empty() or t.last().opt)
		t.push(Command()); // new cmd
	string b;
	char c;
	while ((c = *i++)) {
		if (isspace(c)) continue;
		if (c == ';' ) parse_op(COMB);
		if (c == '|' ) parse_op(PIPE);
		if (c == '&' ) parse_op(JOBS);
		if (c == '<' ) parse_op(RDR_R);
		if (c == '>' ) {
			if (*i == '>') {
				++i;
				parse_op(RDR_A);
			} else parse_op(RDR_W);
		}
		return (--i, C_PARSE_BUF(word));
	}
	if (t.last().args.empty())
		t.pop(); // clean up last cmd
	return t.integrated() ? END : CONT;
}

#undef D_PARSE
#undef D_PARSE_BUF
#undef C_PARSE
#undef C_PARSE_BUF

/*
 * Neither parse_env() nor parse_var() is a part of the parsing circle.
 *
 * parse_env() modifies the input to notify its caller, and returns a
 * partial result.
 *
 * parse_var() is an interface to non-parsing code. It parses and expends
 * an environment variable in the input.
 */

cstr_t parse_env(source_ref i) {
	string s;
	char c = *i;
	if (isdigit(c))
		while ((c = *i) and isdigit(c))
			s.push_back(*i++);
	else if (isalpha(c) or c == '_')
		while ((c = *i) and (isalnum(c) or c == '_'))
			s.push_back(*i++);
	return Shell::getvar(s.c_str());
}

cstr_t parse_var(source_type i) {
	static string b; // uses a static buffer
	b.clear();
	char c;
	while ((c = *i++))
		if (c == '$')
			b.append(parse_env(i));
		else b.push_back(c);
	return b.c_str();
}

}

