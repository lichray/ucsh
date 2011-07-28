/*-
 * Copyright (c) 2011
 *	Zhihao Yuan.  All rights reserved.
 *
 * See the LICENSE file for redistribution information.
 */

#ifndef EDITLINE_H_
#define EDITLINE_H_

#include <histedit.h>
#include "builtin.h"
#include "parser.h"

#ifdef USE_HIST_FILE
#define HIST_FILE (parse_var("$HOME/.ucsh_hist"))
#endif

namespace ucsh {

struct EditLine {
	EditLine(cstr_t prog) : el(el_init(prog, stdin, stdout, stderr)),
			hl(history_init()), ev() {
		el_source(el, NULL);
		el_set(el, EL_PROMPT, &EditLine::prompt);
		el_set(el, EL_HIST, history, hl);
#ifdef HIST_FILE
		history(hl, ev, H_LOAD, HIST_FILE);
#endif
	}

	cstr_t readline(cstr_t s) {
		el_set(el, EL_CLIENTDATA, s);
		int i;
		char* ln = const_cast<char*>(el_gets(el, &i));
		if (i > 0)
			// behaves like GNU readline()
			ln[i-1] = '\0';
		return ln;
	}

	void enter(cstr_t s) {
		history(hl, ev, H_ENTER, s);
	}

	~EditLine() throw () {
#ifdef HIST_FILE
		history(hl, ev, H_SAVE, HIST_FILE);
#endif
		history_end(hl);
		el_end(el);
	}

private:
	::EditLine* el;
	::History* hl;
	::HistEvent ev[1];

	static cstr_t prompt(::EditLine* el) {
		cstr_t s;
		el_get(el, EL_CLIENTDATA, &s);
		return parse_var(getenv(s));
	}

	EditLine(EditLine const&);
	EditLine& operator=(EditLine&);
};

}

#endif
