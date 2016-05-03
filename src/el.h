/*	$NetBSD: el.h,v 1.25 2011/07/29 23:44:44 christos Exp $	*/

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
 *
 *	@(#)el.h	8.1 (Berkeley) 6/4/93
 */

/*
 * el.h: Internal structures.
 */
#ifndef _h_el
#define	_h_el
/*
 * Local defaults
 */
#define	KSHVI
#define	VIDEFAULT
#define	ANCHOR

#include "histedit.h"
#include "chartype.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

#define	EL_BUFSIZ	((size_t)1024)	/* Maximum line size		*/

#define	HANDLE_SIGNALS	0x01
#define	NO_TTY		0x02
#define	EDIT_DISABLED	0x04
#define	UNBUFFERED	0x08
#define	CHARSET_IS_UTF8 0x10
#define	IGNORE_EXTCHARS 0x20            /* Ignore characters read > 0xff */
#define	NARROW_HISTORY	0x40
#define	NARROW_READ	0x80

typedef int bool_t;			/* True or not			*/

typedef unsigned char el_action_t;	/* Index to command array	*/

typedef struct coord_t {		/* Position on the screen	*/
	int	h;
	int	v;
} coord_t;

typedef struct el_line_t {
	Char 		*buffer;	/* Input line			*/
	Char	        *cursor;	/* Cursor position		*/
	Char	        *lastchar;	/* Last character		*/
	const Char	*limit;		/* Max position			*/
} el_line_t;

/*
 * Editor state
 */
typedef struct el_state_t {
	int		inputmode;	/* What mode are we in?		*/
	int		doingarg;	/* Are we getting an argument?	*/
	int		argument;	/* Numeric argument		*/
	int		metanext;	/* Is the next char a meta char */
	el_action_t	lastcmd;	/* Previous command		*/
	el_action_t	thiscmd;	/* this command 		*/
	Char		thisch;		/* char that generated it	*/
} el_state_t;

/*
 * Until we come up with something better...
 */
#define	el_malloc(a)	malloc(a)
#define	el_realloc(a,b)	realloc(a, b)
#define	el_free(a)	free(a)

#include "tty.h"
#include "prompt.h"
#include "keymacro.h"
#include "terminal.h"
#include "refresh.h"
#include "chared.h"
#include "common.h"
#include "search.h"
#include "hist.h"
#include "map.h"
#include "parse.h"
#include "sig.h"
#include "help.h"
#include "read.h"
#include "highlight.h"

struct editline {
	Char		 *el_prog;	/* the program name		*/
	FILE		 *el_infile;	/* Stdio stuff			*/
	FILE		 *el_outfile;	/* Stdio stuff			*/
	FILE		 *el_errfile;	/* Stdio stuff			*/
	int		  el_infd;	/* Input file descriptor	*/
	int		  el_outfd;	/* Output file descriptor	*/
	int		  el_errfd;	/* Error file descriptor	*/
	int		  el_flags;	/* Various flags.		*/
	int		  el_errno;	/* Local copy of errno		*/
	coord_t		  el_cursor;	/* Cursor location		*/
	Char		**el_display;	/* Real screen image = what is there */
	Char		**el_vdisplay;	/* Virtual screen image = what we see */
	void		 *el_data;	/* Client data			*/
	el_line_t	  el_line;	/* The current line information	*/
	el_state_t	  el_state;	/* Current editor state		*/
	el_terminal_t	  el_terminal;	/* Terminal dependent stuff	*/
	el_tty_t	  el_tty;	/* Tty dependent stuff		*/
	el_refresh_t	  el_refresh;	/* Refresh stuff		*/
	el_prompt_t	  el_prompt;	/* Prompt stuff			*/
	el_prompt_t	  el_rprompt;	/* Prompt stuff			*/
	el_prompt_t	  el_mprompt;	/* Prompt stuff			*/
	el_chared_t	  el_chared;	/* Characted editor stuff	*/
	el_map_t	  el_map;	/* Key mapping stuff		*/
	el_keymacro_t	  el_keymacro;	/* Key binding stuff		*/
	el_history_t	  el_history;	/* History stuff		*/
	el_search_t	  el_search;	/* Search stuff			*/
	el_signal_t	  el_signal;	/* Signal handling stuff	*/
	el_read_t	  el_read;	/* Character reading stuff	*/
#ifdef WIDECHAR
	ct_buffer_t       el_scratch;   /* Scratch conversion buffer    */
	ct_buffer_t       el_lgcyconv;  /* Buffer for legacy wrappers   */
	LineInfo          el_lgcylinfo; /* Legacy LineInfo buffer       */
#endif
	el_highlight_t    el_highlight; /* Color highlighting stuff     */
};

protected int	el_editmode(EditLine *, int, const Char **);

#ifdef DEBUG
#define	EL_ABORT(a)	do { \
				fprintf(el->el_errfile, "%s, %d: ", \
					 __FILE__, __LINE__); \
				fprintf a; \
				abort(); \
			} while( /*CONSTCOND*/0);
#else
#define EL_ABORT(a)	abort()
#endif

/* sizeof ( union { T array[N]; TA aligner; } ) */
#define EL_ARRAY_SIZE(T, N, TA) \
	((sizeof(T)*(size_t)(N) + __alignof__(TA) - 1) & \
	 ~(size_t)(__alignof__(TA) - 1))

/*
 * A display (i.e. el_display, el_vdisplay) is an array of pointers to
 * a line character data.
 *
 * A character in position X,Y is stored in el_display[Y][X].
 * Graphic attributes (if COLOR enabled) are in
 *   ((GrParams *) el_display[Y] ) [ -X - 1]
 *
 * In other words, line memory layout is as follows:
 *
 *                      el_display[i]
 *                    /
 *   GRn ... GR1 GR0 | CHAR0 CHAR1 ... CHARn
 *   ---------------   ---------------------
 *   EL_DISPLAY_GR_SIZE EL_DISPLAY_CHAR_SIZE
 *
 * Lines go in memory without gaps, enables mapping a Char * pointer to
 * GR data (2 displays to consider, simple range check tells if a
 * pointer belongs to a display).
 */

/* char data size (one display line) */
#define EL_DISPLAY_CHAR_SIZE(COLS) \
	EL_ARRAY_SIZE(Char, COLS, GrParams)

/* gr data size (one display line) */
#ifdef COLOR
#define EL_DISPLAY_GR_SIZE(COLS) \
	EL_ARRAY_SIZE(GrParams, COLS, Char)
#else
#define EL_DISPLAY_GR_SIZE(COLS) 0
#endif

/*
 * EL_GRPARAMS(el, el_dislay,   Y, X)
 * EL_GRPARAMS(el, el_vdisplay, Y, X) - get/set GR attributes in X,Y
 *                                      cell on either display
 */
#define EL_GRPARAMS(el, name, v_, h_) \
	(((GrParams *)((el)->name[(v_)]))[-1-(ptrdiff_t)(h_)])

/*
 * Lookup GR attributes by a Char * pointer. Considers el_display and
 * el_vdisplay.
 *
 * Note: GR attributes are in reverse order.
 *
 *  el_grparams(c) [ 0] <--> c [0]
 *  el_grparams(c) [-1] <--> c [1]
 *  el_grparams(c) [-2] <--> c [2]
 */
private inline const GrParams *
el_grparams(EditLine *el, const Char *cp)
{
#ifdef COLOR
	coord_t c = el->el_terminal.t_size;
	size_t line_size =
	    EL_DISPLAY_GR_SIZE(c.h + 1) +
	    EL_DISPLAY_CHAR_SIZE(c.h + 1);
	/*
	 * Origin == original line #0; el_display[0] may store a
	 * different value due to buffer scrolling.
	 */
	size_t origin_offset =
	    sizeof(el->el_display[0]) * (size_t)(c.v + 1) +
	    EL_DISPLAY_GR_SIZE(c.h + 1);
	char *origin  = (char *)el->el_display + origin_offset;
	char *vorigin = (char *)el->el_vdisplay + origin_offset;
	size_t index;
	if ((char *)cp >= origin && (char *)cp < origin + line_size * c.v) {
		;
	} else if ((char *)cp >= vorigin && (char *)cp < vorigin + line_size * c.v) {
		origin = vorigin;
	} else {
		return NULL;
	}
	index = (((char *)cp - origin) % line_size) / sizeof(Char);
	if (index > c.h)
		return NULL;
	return (const GrParams *)(cp - index) - index - 1;
#endif
	return NULL;
}

private inline GrParams gr_default()
{
	GrParams gr = {};
	gr.fgcolor = 1;
	return gr;
}

#endif /* _h_el */
