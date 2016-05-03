#include "config.h"
#include <stdlib.h>
#include "el.h"
#include "highlight.h"

private int
highlight_default(EditLine *el, GrParams *gr, size_t count, int mode)
{
	(void)el;
	(void)gr;
	(void)count;
	(void)mode;
	return 0;
}

protected int
highlight_all(EditLine *el)
{
#ifdef COLOR
	el_hfunc_t hfunc = el->el_highlight.h_func;
	int wide = el->el_highlight.h_wide;
	int nocursor = el->el_highlight.h_nocursor;
	size_t size, i;
	size_t capacity;
	GrParams *buf;
	el->el_highlight.h_nocursor = 0;
	if (hfunc == highlight_default)
		return -1;
	size = el->el_line.lastchar - el->el_line.buffer;
	if (wide == 0)
		size *= 4; /* assume UTF-8 */
	capacity = el->el_highlight.h_capacity;
	buf = el->el_highlight.h_buf;
	if (size > capacity) {
		capacity = 512;
		while (size > capacity)
			capacity *= 2;
		buf = el_realloc(buf, capacity * (sizeof buf[0]));
		if (buf == NULL)
			return -1;
		el->el_highlight.h_buf = buf;
		el->el_highlight.h_capacity = capacity;
	}
	for (i = 0; i < size; i++)
		buf[i] = gr_default();
	if (hfunc(el, buf, size, nocursor ? 4 : 1) != 0)
		return -1;
	if (wide == 0) {
		GrParams *i = buf, *o = buf, *e = buf + size;
		Char *ci = el->el_line.buffer, *ce = el->el_line.lastchar;
		while (ci != ce) {
			char dummy[16];
			if (i >= e)
				return -1; /* insufficient buffer */
			*o++ = *i;
			i += ct_encode_char(dummy, sizeof dummy, *ci++);
		}
	}
	return 0;
#else
	(void)el;
	return -1;
#endif
}

protected int
highlight_fastaddc(EditLine *el, GrParams *gr)
{
#ifdef COLOR
	return el->el_highlight.h_func(el, gr, 1, 2);
#else
	(void)el;
	*gr = gr_default();
	return 0;
#endif
}

protected int
highlight_refresh_cursor(EditLine *el)
{
#ifdef COLOR
	return el->el_highlight.h_func(el, NULL, 0, 3);
#else
	(void)el;
	return 0;
#endif
}

protected int
highlight_nocursor(EditLine *el)
{
#ifdef COLOR
	return el->el_highlight.h_func(el, NULL, 0, 5);
#else
	(void)el;
	return 0;
#endif
}

protected int
highlight_set(EditLine *el, el_hfunc_t hfunc, int wide)
{
	el->el_highlight.h_func = hfunc ? hfunc : highlight_default;
	el->el_highlight.h_wide = wide;
	return 0;
}

protected int
highlight_get(EditLine *el, el_hfunc_t *hfunc)
{
	*hfunc = el->el_highlight.h_func;
	if (*hfunc == highlight_default)
		*hfunc = NULL;
	return 0;
}

protected int
highlight_init(EditLine *el)
{
	el->el_highlight.h_func = highlight_default;
	el->el_highlight.h_wide = 1;
	el->el_highlight.h_nocursor = 0;
	el->el_highlight.h_buf = NULL;
	el->el_highlight.h_capacity = 0;
	return 0;
}

protected void
highlight_end(EditLine *el)
{
	el_free(el->el_highlight.h_buf);
}
