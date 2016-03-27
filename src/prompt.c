/*	$NetBSD: prompt.c,v 1.20 2011/07/29 15:16:33 christos Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Christos Zoulas of Cornell University.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#if !defined(lint) && !defined(SCCSID)
#if 0
static char sccsid[] = "@(#)prompt.c	8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: prompt.c,v 1.20 2011/07/29 15:16:33 christos Exp $");
#endif
#endif /* not lint && not SCCSID */

/*
 * prompt.c: Prompt printing functions
 */
#include <stdio.h>
#include "el.h"

private Char	*prompt_default(EditLine *);
private Char	*prompt_default_r(EditLine *);

/* prompt_default():
 *	Just a default prompt, in case the user did not provide one
 */
private Char *
/*ARGSUSED*/
prompt_default(EditLine *el __attribute__((__unused__)))
{
	static Char a[3] = {'?', ' ', '\0'};

	return a;
}


/* prompt_default_r():
 *	Just a default rprompt, in case the user did not provide one
 */
private Char *
/*ARGSUSED*/
prompt_default_r(EditLine *el __attribute__((__unused__)))
{
	static Char a[1] = {'\0'};

	return a;
}

/*
 * Processing SGR escape sequences, e.g. ESC[1;2;4;5m
 */
#ifdef COLOR
private GrParams
update_gr(GrParams gr, Char *x, Char *end)
{
	enum { ARGS_MAX = 32 };
	int args[ARGS_MAX];

	for (; x + 2 < end; x++) {

		int nargs, i;

		if (x[0] != '\033' || x[1] != '[')
			continue;

		args[0] = 0; nargs = 1;
		for ( x += 2; x != end; x++ ) {
			if (*x >= '0' && *x <= '9') {
				args[nargs - 1] =
					args[nargs - 1] * 10 + (*x - '0');
			} else if (*x == ';') {
				if (nargs == ARGS_MAX)
					break;
				args[nargs++] = 0;
			} else {
				break;
			}
		}

		/* invalid sequence */
		if (x == end || *x != 'm')
			continue;

		for (i = 0; i < nargs; i++) {
			switch (args[i]) {
			default:
				/* unrecognized argument */
				i = nargs;
				break;
			case 0:
				gr = gr_default();
				break;
			case 1:
				gr.bold = 1;
				break;
			case 2:
				break;
			case 3:
				gr.italic = 1;
				break;
			case 4:
				gr.underline = 1;
				break;
			case 5:
				gr.blink = 1;
				break;
			case 6:
				break;
			case 7:
				gr.inverse = 1;
				break;
			case 8: case 9: case 10: case 11:
			case 12: case 13: case 14: case 15:
			case 16: case 17: case 18: case 19:
			case 20:
				break;
			case 21: case 22:
				gr.bold = 0;
				break;
			case 23:
				gr.italic = 0;
				break;
			case 24:
				gr.underline = 0;
				break;
			case 25:
				gr.blink = 0;
				break;
			case 27:
				gr.inverse = 0;
				break;
			case 28: case 29:
				break;
			case 30: case 31: case 32: case 33:
			case 34: case 35: case 36: case 37:
				gr.fgcolor = args[i] - 30;
				break;
			case 38:
				if (i + 2 >= nargs ||
					(args[i + 1] != 2 && args[i + 1] != 5)) {
					/* unrecognized argument */
					i = nargs;
					break;
				}
				if (args[i + 1] == 2) {
					/* true color not supported */
					gr.fgcolor = 1;
					i += 4;
				} else {
					gr.fgcolor = args[i + 2];
					i += 2;
				}
				break;
			case 40: case 41: case 42: case 43:
			case 44: case 45: case 46: case 47:
				gr.bgcolor = args[i] - 40;
				break;
			case 48:
				if (i + 2 >= nargs ||
					(args[i + 1] != 2 && args[i + 1] != 5)) {
					/* unrecognized argument */
					i = nargs;
					break;
				}
				if (args[i + 1] == 2) {
					/* true color not supported */
					gr.bgcolor = 1;
					i += 4;
				} else {
					gr.bgcolor = args[i + 2];
					i += 2;
				}
				break;
			case 51: case 52: case 53: case 54: case 55:
			case 60: case 61: case 62: case 63: case 64: case 65:
				break;
			}
		}
	}
	return gr;
}
#endif

/* prompt_print():
 *	Print the prompt and update the prompt position.
 */
protected void
prompt_print(EditLine *el, int op)
{
	el_prompt_t *elp;
	GrParams gr;
	Char *p;
	Char *x = NULL;
	Char ignore;

	if (op == EL_PROMPT)
		elp = &el->el_prompt;
	else
		elp = &el->el_rprompt;

	if (elp->p_wide)
		p = (*elp->p_func)(el);
	else
		p = ct_decode_string((char *)(void *)(*elp->p_func)(el),
		    &el->el_scratch);

	gr = gr_default();
	ignore = elp->p_ignore;
	for (; *p; p++) {
		if (*p == ignore) {
			if (x == NULL) {
				x = p + 1;
			} else {
#ifdef COLOR
				gr = update_gr(gr, x, p);
#endif
				x = NULL;
			}
			continue;
		}
		if (x == NULL)
			re_addc(el, gr, *p);
	}

	elp->p_pos.v = el->el_refresh.r_cursor.v;
	elp->p_pos.h = el->el_refresh.r_cursor.h;
}


/* prompt_init():
 *	Initialize the prompt stuff
 */
protected int
prompt_init(EditLine *el)
{

	el->el_prompt.p_func = prompt_default;
	el->el_prompt.p_pos.v = 0;
	el->el_prompt.p_pos.h = 0;
	el->el_prompt.p_ignore = '\0';
	el->el_rprompt.p_func = prompt_default_r;
	el->el_rprompt.p_pos.v = 0;
	el->el_rprompt.p_pos.h = 0;
	el->el_rprompt.p_ignore = '\0';
	return 0;
}


/* prompt_end():
 *	Clean up the prompt stuff
 */
protected void
/*ARGSUSED*/
prompt_end(EditLine *el __attribute__((__unused__)))
{
}


/* prompt_set():
 *	Install a prompt printing function
 */
protected int
prompt_set(EditLine *el, el_pfunc_t prf, Char c, int op, int wide)
{
	el_prompt_t *p;

	if (op == EL_PROMPT || op == EL_PROMPT_ESC)
		p = &el->el_prompt;
	else
		p = &el->el_rprompt;

	if (prf == NULL) {
		if (op == EL_PROMPT || op == EL_PROMPT_ESC)
			p->p_func = prompt_default;
		else
			p->p_func = prompt_default_r;
	} else {
		p->p_func = prf;
	}

	p->p_ignore = c;

	p->p_pos.v = 0;
	p->p_pos.h = 0;
	p->p_wide = wide;

	return 0;
}


/* prompt_get():
 *	Retrieve the prompt printing function
 */
protected int
prompt_get(EditLine *el, el_pfunc_t *prf, Char *c, int op)
{
	el_prompt_t *p;

	if (prf == NULL)
		return -1;

	if (op == EL_PROMPT)
		p = &el->el_prompt;
	else
		p = &el->el_rprompt;

	if (prf)
		*prf = p->p_func;
	if (c)
		*c = p->p_ignore;

	return 0;
}
