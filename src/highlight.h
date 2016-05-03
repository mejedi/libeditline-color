/*
 * el.highlight.h: Color highlighting stuff
 */
#ifndef _h_el_highlight
#define	_h_el_highlight

#include "histedit.h"

typedef int    (*el_hfunc_t)(EditLine *, GrParams *, size_t, int mode);

typedef struct {
	el_hfunc_t	h_func;
	int		h_wide;
	int		h_nocursor;
	GrParams       *h_buf;
	size_t          h_capacity;
} el_highlight_t;

protected int	highlight_all(EditLine *); /* mode = 1 or 4 (nocursor) */
protected int   highlight_fastaddc(EditLine *, GrParams *); /* mode = 2 */
protected int   highlight_refresh_cursor(EditLine *); /* mode = 3 */
protected int   highlight_nocursor(EditLine *); /* mode = 5 */
protected int	highlight_set(EditLine *, el_hfunc_t, int);
protected int	highlight_get(EditLine *, el_hfunc_t *);
protected int	highlight_init(EditLine *);
protected void	highlight_end(EditLine *);

#endif /* _h_el_highlight */
